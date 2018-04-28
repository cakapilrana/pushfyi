/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI TCPReader

*********************************************************
*/
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <uuid/uuid.h>
#include "TCPReader.h"
#include "Log.h"
#include "Properties.h"
#include "ProcessInit.h"
#include "Pushfyi-Defs.h"
#include "ClientFactory.h"

void TCPReader::initializeCommandMap()
{
	/*
	 * TODO:
	 * TCPReader Stats for management console.
	 */
    mCommandMap[TCP_READER_REPORT_STATUS] = eStatus;

    /*
     * New Client Connection From SecureTCPServer
     */
    mCommandMap[TCP_READER_NEW_PUSHFYI_CLIENT] = eNewClientConnection;

    /*
     * Command to cleanup dead clients
     */
    mCommandMap[TCP_READER_CLEAN_UP] = eCleanUp;
}

TCPReader::TCPReader(string topic) : EpollProxy(DEFAULT_MAX_CLIENTS_10M), mThread(0), mFinished(false),
                						mTopic(topic),	mRouter(PubSubRouter::getRouter())
{
    /*
     *  Initialize TCPReader's Command Map
     */
    initializeCommandMap();

    /*
     * Check node name
     */
    Properties* sysProps = ProcessInit::instance()->getSysProps();
    if(sysProps->get("run.mode") == RUN_MODE_CLUSTER)
    	mNodeName = sysProps->get("node.name");
    else
    	mNodeName = RUN_MODE_STANDALONE;

    /*
     *  Allocate the TCPReader's command queue
     */
    mCmdQueue       = new PriorityQueue<Event>(mTopic, "command", 100000);

    /*
     * Initialize the SSL Context
     */

    /*
     * Initialize the fd set and open the notification pipe
     */
    if( pipe(mPipeFd) ) {
		//error creating pipe
		ERROR(Log::eTCPReader, "Error creating pipe. TCPreader can not be sent commands.");
    } else {
        //add to EPOLL SET
		if(addEpollFd(mPipeFd[0]) == 0)
            INFO(Log::eTCPReader, "Notification pipe created. Adding fd = %d to select fd set", mPipeFd[0]);
        else
            ERROR(Log::eTCPReader, "Notification pipe failed to add to EpollProxy. TCPreader can not be sent commands.");
    }

    /*
     * Initialize Configurables
     */
    mEpollWaitPeriod = TCPReader::DEFAULT_EPOLL_TIME_OUT_MILLIS;
}

TCPReader::~TCPReader()
{
    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eTCPReader, "TCPReader Thread stoped");

        //Close notification pipe
        ::close(mPipeFd[0]);
        ::close(mPipeFd[1]);
    }

    //delete the command queue
    if(mCmdQueue != NULL){
        delete mCmdQueue;
    }
}

bool TCPReader::online()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        INFO(Log::eTCPReader, "TCPReader Thread already running");
        return false;
    }

    subscribe();

    mFinished = false;

    mThread = new Thread(this);
    mThread->start();

    INFO(Log::eTCPReader, "TCPReader Thread started");

    /*
     * Notify Listeners of the reader availability
     */
    Event notify = new EventRep(PUSHFYI_TCP_SERVER_TOPIC, TCP_READER_ONLINE);
    notify->set(PUSHFYI_MODULE_COMMAND, TCP_READER_ONLINE);
    notify->set("reader", mTopic);

    mRouter.publish(notify);

    notify->setTopic(PUSHFYI_TLS_SERVER_TOPIC);
    mRouter.publish(notify);

    return bRet;
}

bool TCPReader::offline()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eTCPReader, "TCPReader Thread stoped");
    }

    return bRet;
}

void TCPReader::idle(int ret)
{
	INFO(Log::eTCPReader, "TCPReader alive and idling");
}

/*
 *  TCPReader Main Listener Thread. All TCP connections must come here.
 *  Client sockets shall than be dispatched to TCPWorker for async io
 */
void TCPReader::run()
{
    rxInt ret = 0;
	while(!mFinished) {
        /*
         *  Time to wait for events.
         */
        ret = EpollWait(mEpollWaitPeriod);
        if( ret == -1 ) {
            ERROR(Log::eTCPReader, "error occured on epoll_wait, return = %d", ret);
            //best is to continue for now
            continue;
        }

        if( ret == 0 ) {
        	idle(ret);
        }

        else if( ret > 0) {
            //some events have been triggered
        	INFO(Log::eNetwork, "EPOLL EVENTS found = %d", ret);

            for(rxInt i=0; i < ret; i++) {
                epoll_event* ev = getEvent(i);

                //check for error notifications
                if( (ev->events & EPOLLERR) || (ev->events & EPOLLHUP) ) {
                    /* An error has occured on this fd, or the socket is not
                    ready for reading (why were we notified then?) */

                	PushFYIClient* pSocket = (PushFYIClient*)ev->data.ptr;
                	if(pSocket)
                	{
						int       error = 0;
						socklen_t errlen = sizeof(error);
						if (getsockopt(pSocket->getSocket(), SOL_SOCKET, SO_ERROR, (void *)&error, &errlen) == 0)
						{
							ERROR(Log::eTCPReader, "Error occurred on EPOLL fd = %d %s", pSocket->getSocket(), strerror(error));
						}
						expire(pSocket);
						continue;
                	}
                	else
                	{
                		ERROR(Log::eTCPReader, "Invalid socket object. Check if its the notification pipe.");
                		/*
                		 * TODO: Handle notification pipe close gracefully.
                		 *
                		 * Try to re open it. Because we want to be able to send
                		 * commands to TCPReader Always at Any given point of time.
                		 */
                        if(ev->data.fd == mPipeFd[0]) {
                            ERROR(Log::eTCPReader, "TODO: Notification pipe closed. Fix it.");
                        }
                	}
                }

                //check for notification pipe
                if(ev->data.fd == mPipeFd[0]) {
                    INFO(Log::eTCPReader, "TCPReader received pushfyi command.");
                    char buff[10] = {0};
                    //clear the data in the pipe.
                    ::read(mPipeFd[0], buff, sizeof(buff));
                    processCmd();
                }
                //this is one of the clients
                else {
                    /*
                     *  We Have A Read Event. This could mean a few things:
                     *  1. Push FYI HTTP Upgrade Request
                     *  2. Push FYI SSL Handshake - SSLv3 Client
                     *  3. publish and subscribe key validation request ie. Hello Message
                     *  4. incoming subscribe request
                     *  5. incoming publish request
                     */
                	//Get the EventSocket
                	PushFYIClient* pSocket = (PushFYIClient*)ev->data.ptr;

                	if(pSocket->getSecurity() == "unknown")
                	{
                		/*
                		 * This is a client with unknown security
                		 */
                		rxInt fd = pSocket->getSocket();

                		string security = "unsupported";
						if( ev->events & EPOLLIN ) {
							/*
							 * Check this message is it is a TLS ClientHello
							 */
							char request[4096] = {0};
							rxInt length = recv(fd, request, 4096 , MSG_PEEK);
							if(length == 0)
								close(fd);
							else {
								/*
								 * Check if it is a TLS or a PLAIN TEXT Client
								 */
							    if((bcmp(request, "\x16", 1) == 0) ||
							               (bcmp(request, "\x80", 1) == 0)) {

							        if(
							        	((bcmp(request + 1, "\x03", 1) == 0) && (bcmp(request + 2, "\x01", 1) == 0)) ||
							        	((bcmp(request + 1, "\x03", 1) == 0) && (bcmp(request + 2, "\x02", 1) == 0)) ||
							        	((bcmp(request + 1, "\x03", 1) == 0) && (bcmp(request + 2, "\x03", 1) == 0))
							        ) {

							        	INFO(Log::eTCPReader, "SSL/ TLS (%d:%d) connection requested", *(request+1), *(request+2));
							        	security = "tls";
							        } else {
							        	INFO(Log::eTCPReader, "Unsupported security scheme (%d.%d) requested.", *(request+1), *(request+2));
							        	close(ev->data.fd);
							        }
							    } else {
							        INFO(Log::eTCPReader, "Plain text connection requested.");
							        security = "text";
							    }

							    /*
							     * Lets create the client
							     */
							    Event newCon = new EventRep(mTopic, TCP_READER_NEW_PUSHFYI_CLIENT);
								newCon->setUInt("fd", fd);
								newCon->set("ip", pSocket->getIP());
								newCon->setUInt("port", pSocket->getPort());
								newCon->set("security", security);

								//first remove this instance
								removeEpollFd(fd);
								pSocket->expire();

								pSocket = processNewClient(newCon);

								if( ! pSocket->isHandshakeComplete())
									processHandshake(pSocket);
							}
						}
                	} else {

						if( ev->events & EPOLLIN ) {

							if( ! pSocket->isHandshakeComplete())
								processHandshake(pSocket);
							/*
							 *  This is an incoming publish or subscribe command.
							 *  Discard anything else.
							 */
							else
								processNewEvent(pSocket);
						}

	                    /*
	                     *  Notification trigger to process, outbound Publications
	                     *  generated on previous subscribe.
	                     *
	                     *  pSocket->process() needs to be optimized.
	                     *  Currently it proceses only one message per trigger.
	                     *  We should be able to publish messages in batches.
	                     */
	                    if( (ev->events & EPOLLOUT) ) {
	                        INFO(Log::eTCPReader, "Internal publish trigger for client on fd = %d", pSocket->getSocket());
	                        if(pSocket->process()==PushFYIClient::FAILED)
	                        {
	                        	expire(pSocket);
	                        }
	                    }
                	}

                } // client notification
            } // loop for all triggered events
            INFO(Log::eNetwork, "EPOLL EVENTS PROCESSED");

            /*
             * Check if something has arrived while we were busy.
             *
             * TODO: Temporarily coomenting this. This may be causing events to miss.

            ret = EpollWait(0);
            if (ret > 0) {
            	mEpollWaitPeriod = 0;
            	INFO(Log::eNetwork, "There are %d pending read fds", ret);
            } else {
            	mEpollWaitPeriod = DEFAULT_EPOLL_TIME_OUT_MILLIS;
            	INFO(Log::eNetwork, "NO pending read items");
            }
            */
        } // epoll returned some events

	} // main thread loop
	ERROR(Log::eNetwork, "Exited epoll wait loop.");
}

void TCPReader::processCmd()
{
    static const rxInt MAX_NO_CMD_TO_PROCESS = 4;
    bool moreCmdsToProcess = false;

    rxInt n = mCmdQueue->getEntryCount();

    if(n > MAX_NO_CMD_TO_PROCESS) {
        n = MAX_NO_CMD_TO_PROCESS;
        moreCmdsToProcess = true;
    }

    for(rxInt i = 0; i < n; i++){
        Event ev = mCmdQueue->pop();

        string cmd = ev->get("command");
        if(cmd.empty()){
            WARN(Log::eTCPReader, "Command parameter not set, Topic = %s Subtopic = %s", ev->getTopic().c_str(), ev->getSubtopic().c_str());
            continue;
        }

        INFO(Log::eTCPServer, "Processing command: %s", cmd.c_str());
        switch(mCommandMap[cmd]) {
            case eNewClientConnection:
            {
                processNewClient(ev);
            }
            break;
            case eHello:
            {
            }
            break;
			case eStatus:
			{

			}
			break;

			case eCleanUp:
			{
				//cleanup();
			}
			break;

			default:
			{
                WARN(Log::eTCPReader, "Unknown Command = %s, Topic = %s Subtopic = %s", cmd.c_str(), ev->getTopic().c_str(), ev->getSubtopic().c_str());
                break;
			}
        } //end of switch
    }

    if( moreCmdsToProcess ) {
    	notify();
    }
}

void TCPReader::processHandshake(PushFYIClient* pSocket)
{
	string ip = pSocket->getIP();
	rxUInt port = pSocket->getPort();
	rxUInt fd = pSocket->getSocket();

	INFO(Log::eTCPReader, "Processing handshake from client {%s:%d} on fd = %d", ip.c_str(), port, fd);

	//start the handshake
	if( ! pSocket->isHandshakeComplete() ) {
		PushFYIClient::TASK_STATUS ret = pSocket->doHandshake();
		switch(ret) {
			case PushFYIClient::FAILED:
			{
				/*terminate this connection*/
				INFO(Log::eTCPReader, "Secure handshake has failed on fd = %d", fd);
				//removeEpollFd(fd);
				//TODO: memory leak - delete pSocket;
				//close(fd);

				expire(pSocket);
			}
			break;
			case PushFYIClient::COMPLETED:
			{
				INFO(Log::eTCPReader, "Handshake complete on fd = %d", fd);

				/*
				 * This could be the place to start the HB
				 */
				if(pSocket->getProtocol() == PUSHFYI_PROTOCOL::eProtocolWebSocket ||
						pSocket->getProtocol() == PUSHFYI_PROTOCOL::eProtocolStreamSocket) {

					pSocket->startHeartBeatTimer(20*1000);
				}
			}
			break;

			case PushFYIClient::INPROGRESS:
			{
				INFO(Log::eTCPReader, "SSL Handshake is awaiting io on fd = %d", fd);
			}
			break;
		}
	}
}

void TCPReader::notify()
{
    //if the write pipe fd is open, write 1 char for select to wake up
    if(mPipeFd[1] > 0) {
        const char* c = "x";
        ::write(mPipeFd[1], c, 1);
        INFO(Log::eTCPReader, "TCPReader command notified.");
    } else {
        ERROR(Log::eTCPReader, "TCPReader notification pipes not open. Command notification failed.");
    }
}

void TCPReader::push(Event event)
{
    submitCmd(event);
}

void TCPReader::pushFront(Event event)
{
    submitCmd(event, true);
}

void TCPReader::submitCmd(Event event, bool isHighPriority)
{
    if (isHighPriority) {
        mCmdQueue->pushFront(event);
    }
    else {
        mCmdQueue->push(event);
    }

    notify();
}

void TCPReader::subscribe()
{
    mRouter.subscribe("", "", this, mTopic, "", -1);
}

void TCPReader::unsubscribe()
{
    mRouter.unsubscribe("", "", this, mTopic, "");
}

PushFYIClient* TCPReader::processNewClient(Event& ev)
{
	string ip = ev->get("ip");
	rxUInt port = ev->getUInt("port");
	rxUInt fd = ev->getUInt("fd");

	ev->setUInt("epoll", getEpollInstance());

    INFO(Log::eTCPReader, "new connection request from client {%s:%d} on fd = %d", ip.c_str(), port, fd);

    PushFYIClient* pClient = 0;

    /*
     * Create a PushFYIClient For this Client
     *
     * fd - Client's fd as returned by call to accept
     * notificationFD - Instance of this EpollProxy - used for outbound notifications
     * ip and ephimeral port of the client
     */
    ClientFactory* factory = ClientFactory::getInstance();
    pClient = factory->CreateClient(ev);

    if(pClient) {
    	INFO(Log::eTCPReader, "EventSocket created for client on fd = %d", fd);

    	/*
    	 * Add this client to our watch list.
    	 */
    	if(addEpollFd(pClient) < 0)
    	{
    		ERROR(Log::eTCPReader, "Failed to add to epollset client fd = %d, on ip = %s", fd, ip.c_str());
    		close(fd);
    	}
    	INFO(Log::eTCPReader, "We are now watching client on fd = %d, ip=%s", fd, ip.c_str());

    }

    else {
    	INFO(Log::eTCPReader, "Couldnt create PushFYIClient for fd=%d, unknown security scheme", fd);
    	/*
    	 * Add this client to our watch list.
    	 *
    	 * We will create a client when we will
    	 * get to know the security model.
    	 */
    	if(addEpollFd(fd) < 0)
    	{
    		ERROR(Log::eTCPReader, "Failed to add to epollset client fd = %d, on ip = %s", fd, ip.c_str());
    		close(fd);
    	}
    	INFO(Log::eTCPReader, "We are now watching client on fd = %d, ip=%s", fd, ip.c_str());
    }

    return pClient;
}

void TCPReader::processNewEvent(PushFYIClient* pSocket)
{
    INFO(Log::eTCPReader, "Read event detected on socket = %d", pSocket->getSocket());
    Event ev = NULL;
    PushFYIClient::TASK_STATUS ret = pSocket->read(ev);

    switch(ret)
    {
    	case PushFYIClient::FAILED:
    	{
    		expire(pSocket);
            return;
    	}
    	break;

    	case PushFYIClient::INPROGRESS:
    	{
    		if(ev.isNull())
    			return;
    	}
    	break;

    	case PushFYIClient::COMPLETED:
    	{
    		if(ev.isNull())
    			return;
    	}
    	break;
    };

    if(ev->isCompound())
    {
    	EventList list = (EventListRep*)*ev;

    	for(rxUInt i=0; i<list->size();i++)
    	{
    		Event e = list->getEvent(i);
    		int ret = processClientCommand(pSocket, e);

    		if(ret < 0)
    			break;
    	}
    }

    else {
    	processClientCommand(pSocket, ev);
    }

}

int TCPReader::processClientCommand(PushFYIClient* pSocket, Event& ev)
{
    string cmd = ev->get("command");
    if(cmd.empty()) {
        WARN(Log::eTCPReader, "Command parameter not set, Topic = %s Subtopic = %s", ev->getTopic().c_str(), ev->getSubtopic().c_str());
        return 1;
    }

    //check if it is publication or a subscription
    if(cmd == "publish") {
        /*
         *  Publish this message as is
         */
        string subkey = ev->getTopic();
        string channel = ev->getSubtopic();

        INFO(Log::eTCPReader, "Publishing to channel = %s", channel.c_str());
    	pSocket->publish(ev);
    }

    else if(cmd == "subscribe") {
        /*
         *  Create a subscription for this client
         */
        string subkey = ev->getTopic();
        string channel = ev->getSubtopic();

        INFO(Log::eTCPReader, "Subscribing to channel = %s", channel.c_str());
        pSocket->subscribe(subkey, channel);
    }

    else if(cmd == "authorize") {
        /*
         *  WebSocket Upgrade Is Complete and We have a hello message.
         *
         *  1. We should prepare the client for internal signaling.
         *
         *  2. Validate the license key and appsecret
         *
         *	Validation is performed Asynchronously via PushFYIManager.
         *
         *  If keys do not match, just terminate the connection.
         *  without sending any further response or error code.
         *
         *  So, if a client sees that the connection is being closed
         *  by the pushfyi cluster on init request, it would mean
         *  that the keys are wrong.
         *
         *  There is no other better option available at the moment.
         *
         */

    	/*
    	 * Subscribe to client's key pair for internal signaling.
    	 */
    	string appsecret = ev->getTopic();
    	string signalingkey = TCPReader::getUUID();
    	string passkey = ev->get("passkey");

    	/*
    	 * License key and App secret must be
    	 * 36 character long UUID (RFC4122)
    	 */
    	if(appsecret.length()<36)
    	{
    		ERROR(Log::eTCPReader, "App Secret too short. Invalid Mgmt Client.");
    		expire(pSocket);

    		return -1;
    	} else {
			pSocket->setAppSecret(appsecret);
			pSocket->setSignalingKey(signalingkey);

			pSocket->subscribe(appsecret, signalingkey);

			INFO(Log::eTCPReader, "Authorize Management Client appsecret = %s and passkey = %s", appsecret.c_str(), passkey.c_str());

			// forward this request to PushFYIManager and await response.
			ev->setTopic(PUSHFYI_MANAGER_TOPIC);
			ev->setUInt("fd", pSocket->getSocket());

			//reply parameters
			ev->set("appsecret", appsecret);
			ev->set("signalingkey", signalingkey);

			mRouter.publish(ev);
    	}
    }

    else if(cmd == "unsubscribe") {
        /*
         *  Unsubscribe client from channel.
         */
        string subkey = ev->getTopic();
        string channel = ev->getSubtopic();

        INFO(Log::eTCPReader, "Unsubscribe event = \n%s", ev->toString().c_str());
        INFO(Log::eTCPReader, "Unsubscribing from channel = %s", channel.c_str());

        //Do not pass timeout value in unsubscribe for immediate unsubscribe.
        mRouter.unsubscribe("", "", pSocket, subkey, channel);
        //INFO(Log::eTCPReader, "unsubscribe event = \n%s", ev->toString().c_str());
    }

    else if(cmd == "ping") {
    	INFO(Log::eTCPReader, "Client ping on fd=%d", pSocket->getSocket());
    }

    else if(cmd == "pong") {
    	INFO(Log::eTCPReader, "Client sent an application layer PONG on fd = %d", pSocket->getSocket());
    	pSocket->pong();
    }

    else if(cmd == "kill") {
    	INFO(Log::eTCPReader, "Client killed due to missed heart beats = ", pSocket->getSocket());
    	expire(pSocket);
    }

    else if(cmd == "license" ) {
    	INFO(Log::eTCPReader, "License install command received on fd=%d", pSocket->getSocket());

		ev->setTopic(PUSHFYI_MANAGER_TOPIC);
		ev->setUInt("fd", pSocket->getSocket());

		//reply parameters
		//ev->set("appsecret", pSocket->getAppSecret());
		//ev->set("signalingkey", pSocket->getSignalingKey());

		mRouter.publish(ev);
    }

    else {
        /*
         * This is an unknown command.
         * Ignorance is bliss.
         */
    	INFO(Log::eTCPReader, "Unknown command on fd=%d", pSocket->getSocket());
    }

    return 0;
}

/*
 * Time to clean up.
 */
void TCPReader::expire(PushFYIClient* pSocket)
{
	rxUInt fd = pSocket->getSocket();
	INFO(Log::eTCPReader, "Expiring client with fd = %d", fd);

	/*
	 * First disconnect the client ASAP.
	 */
	removeEpollFd(fd);
	close(fd);

	/*
	 * Expire the client
	 */
	pSocket->expire();
}

/**
 * Generate UUID
 */
string TCPReader::getUUID()
{
    char   uuidStr[64] = {0};
    uuid_t uuid;

    ::uuid_generate(uuid);
    ::uuid_unparse(uuid, uuidStr);

    return uuidStr;
}
