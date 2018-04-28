/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part on in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS SSL TCP Server Listener.

*********************************************************
*/
#include <unistd.h>
#include <algorithm>
#include <errno.h>
#include <string.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
#include "TCPServer.h"
#include "PubSubRouter.h"
#include "Log.h"
#include "Properties.h"
#include "ProcessInit.h"
#include "Pushfyi-Defs.h"

TCPServer* TCPServer::mInstance = 0;

TCPServer* TCPServer::getInstance()
{
    if(0 == mInstance) {
        mInstance = new TCPServer();
    }
    return mInstance;
}

void TCPServer::initializeCommandMap()
{
	mCommandMap[REPORT_STATUS] = eStatus;
    mCommandMap[TCP_READER_ONLINE] = eReaderOnline;
}

TCPServer::TCPServer() : EpollProxy(10000), mThread(0), mTopic(""),
                						mRouter(PubSubRouter::getRouter()), mIsAlternatePort(false)
{
    /*
     * Set the network interface to start listening on:
     *
     * 1. If we are running stand alone. This should be a public ip.
     *
     * 2. If we are running in LVS cluster. This should be a static IP from
     * the same physical segment where IPVS Director's local interface is connected.
     */
    Properties* sysProps = ProcessInit::instance()->getSysProps();
    if(sysProps->contains("pushfyirs.tcp.server.host"))
        setHost(sysProps->get("pushfyirs.tcp.server.host"));
    else
    	/*
    	 * default value when the above key is not defined in the config file.
    	 *
    	 * This may be only good for testing on a local machine.
    	 */
        setHost("0.0.0.0");

    /*
     *  Set TCP Listen PORT
     *
     *  1. If we are running stand alone. This should be a public ip.
     *
     *  2. If we are running in LVS cluster. This should be a local port.
     */
    if(sysProps->contains("pushfyirs.tcp.server.port"))
        setPort(sysProps->getUInt("pushfyirs.tcp.server.port"));
    else
        setPort(80);

    /*
     *  Initialize TCPServer's Command Map
     */
    initializeCommandMap();

    /*
     *  Set the TCPServer's command queue topic
     */
    mTopic          = PUSHFYI_TCP_SERVER_TOPIC;

    /*
     *  Allocate the TCPServer's command queue
     */
    mCmdQueue       = new PriorityQueue<Event>(mTopic, "command", 256);


    /*
     * Create Notification Pipe and Add to EPOLL SET
     */
    if( pipe(mPipeFd) ) {
		//error creating pipe
		ERROR(Log::eTCPServer, "Error creating pipe. TCPServer can not be sent commands.");
    } else {
        //add to EPOLL SET
		if(addEpollFd(mPipeFd[0]) == 0)
            INFO(Log::eTCPServer, "Notification pipe created. Adding fd = %d to select fd set", mPipeFd[0]);
        else
            ERROR(Log::eTCPServer, "Notification pipe failed to add to EpollProxy. TCPreader can not be sent commands.");
    }

    /*
     * Empty TCPReader's list
     */
    mReaders.clear();
    mReaderIndex = 0;

    /*
     * Initialize diagnostics
     */
    _StartTime = Time::timeInSeconds();
    _IdleCycles = 0;
    _ConnectionAccepts = 0;
}

TCPServer::~TCPServer()
{
    //unsubscribe from receiving further messages
    unsubscribe();

    if(mThread != 0) {
        //signal the run method to exit
        close();

        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eTCPServer, "TCP Server Thread stoped");

        //Close notification pipe
        ::close(mPipeFd[0]);
        ::close(mPipeFd[1]);
    }

    //delete the command queue
    if(mCmdQueue != NULL){
        delete mCmdQueue;
    }

}

bool TCPServer::online()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        INFO(Log::eTCPServer, "TCP Server Thread already running");
        return false;
    }

    subscribe();

    mFinished = false;

    mThread = new Thread(this);
    mThread->start();

    INFO(Log::eTCPServer, "TCP Server Thread started");

    return bRet;
}

bool TCPServer::offline()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    //unsubscribe from receiving further messages
    unsubscribe();

    if(mThread != 0) {
        //signal the serversocket to close
        close();

        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eTCPServer, "TCP Server Thread stoped");
    }

    return bRet;
}

/*
 *  TCP Server Main Listener Thread. All TCP connections must come here.
 *  Client sockets shall than be dispatched to TCPWorker for async io
 */
void TCPServer::run()
{
	rxStatus ret = bindAndListen();

	if(ret <0) {
		ERROR(Log::eTCPServer, "TCP Server failed to bind, return = %d", ret);
	} else {
		//add listening socket to EPOLL SET
		INFO(Log::eTCPServer, "Adding listening socket %d to epoll set", mSocket);
		addEpollFd(mSocket);
	}

    Accept();
}

void TCPServer::Accept()
{
    INFO(Log::eTCPServer, "Entering epoll loop");

    rxInt ret = 0;
	while(!mFinished) {

		/*
		 * Check if we are listening
		 */
		if(mSocket == -1)
		{
		    rxStatus ret = bindAndListen();

		    if(ret <0) {
		        ERROR(Log::eTCPServer, "TCP Server failed to bind, return = %d", ret);
		    } else {
		        //add listening socket to EPOLL SET
		        INFO(Log::eTCPServer, "Adding listening socket %d to epoll set", mSocket);
		        addEpollFd(mSocket);
		    }
		}
        /*
         *  It's time to wait for incoming connections
         */
        ret = EpollWait(TCPServer::DEFAULT_EPOLL_TIME_OUT_MILLIS);
        if( ret == -1 ) {
            INFO(Log::eTCPServer, "error occured on epoll_wait, return = %d", ret);
            //best is to continue for now
            continue;
        }

        if( ret == 0 ) {
            // this is some idle time.
        	++_IdleCycles;
        	idle(ret);
        }

        else if( ret > 0 ) {
            //some events have been triggered
            for(rxInt i=0; i < ret; ++i) {
                epoll_event* ev = getEvent(i);

                //check for error notifications
                if( (ev->events & EPOLLERR) || (ev->events & EPOLLHUP) ||
                		!(ev->events & EPOLLIN)) {

                	/*
                	 * We have a situation, check which socket.
                	 */
                	if(ev->data.fd == mSocket) {
                        ERROR(Log::eTCPServer, "We are not listening now. Error occurred on EPOLL FD = %d", ev->data.fd);
                        close();
                        removeEpollFd(ev->data.fd);
                        continue;
                	}

                	else if(ev->data.fd == mPipeFd[0]) {
        		        INFO(Log::eTCPServer, "Error occured in notification fd %d", mPipeFd[0]);
                	}
                }

                //check for notification pipe
                if(ev->data.fd == mPipeFd[0]) {
                	INFO(Log::eTCPServer, "TCPServer Received Push FYI Control Command");
                    char buff[10] = {0};
                    //clear the data in the pipe.
                    ::read(mPipeFd[0], buff, sizeof(buff));
                    processCmd();
                }

                //check the server socket for incoming
                if(ev->data.fd == mSocket) {
                    /*
                     *  We Have A New Incoming Connection
                     */
                    struct sockaddr peerAddress;
                    socklen_t socklen = sizeof(peerAddress);

                    int newSocket = accept(mSocket, &peerAddress, &socklen);
                    if(newSocket == -1) {
                        ERROR(Log::eTCPServer, "Error in accept, listen address = %s, port = %d epoll_wait returned = %d", mHost.c_str(), mPort, ret);
                        continue;
                    }

                    rxUShort port = ntohs(*((rxUShort*) &(peerAddress.sa_data[0])));

                    char ip[32] = {0};

                    sprintf(ip, "%hhu.%hhu.%hhu.%hhu", peerAddress.sa_data[2],peerAddress.sa_data[3],peerAddress.sa_data[4],
                            peerAddress.sa_data[5]);

                    INFO(Log::eSecureTCPServer, "eTCPServer received incoming client connection from = %s %d", ip, port);

					/*
					 * Dont check black list, just process this connection
					 */
					processNewClientSocket(newSocket, ip, port);
                }
            }
        }
	}
	::close(mSocket);
	mSocket = -1;
}

void TCPServer::idle(int retval)
{
//	INFO(Log::eTCPServer, "Secure TCP Server Stats: Idle Cycles = %d, Total Accepts = %d", _IdleCycles, _ConnectionAccepts);
}

void TCPServer::processNewClientSocket(int socketFd, char* ip, rxUShort port)
{
	Synchronized lock(mMutex);

	rxUInt readers = mReaders.size();

	if(readers > 0)
	{
		string targetReader = mReaders[mReaderIndex];
		Event newCon = new EventRep(targetReader);

		newCon->set(PUSHFYI_MODULE_COMMAND, TCP_READER_NEW_PUSHFYI_CLIENT);
		newCon->setUInt("fd", socketFd);
		newCon->set("ip", ip);
		newCon->setUInt("port", port);

		/*
		 * if mIsAlternatePort = true
		 * than we are not sure of the security
		 */
		if(!mIsAlternatePort)
			newCon->set("security", "text");
		else
			newCon->set("security", "unknown");

		INFO(Log::eTCPServer, "Sending new client event to TCPReader");
		mRouter.publish(newCon);

		/*
		 * Increment Connection Accept Counter
		 */
		++_ConnectionAccepts;

		/*
		 * Point to next reader
		 */
		mReaderIndex = (mReaderIndex+1)%readers;
	} else {
		WARN(Log::eTCPServer, "TCP Readers are currently not available. Can not accept client connection.");
		::close(socketFd);
	}
}

void TCPServer::processCmd()
{
    static const rxInt MAX_NO_CMD_TO_PROCESS = 4;
    bool moreCmdsToProcess = false;

    rxInt n = mCmdQueue->getEntryCount();

    if(n > MAX_NO_CMD_TO_PROCESS){
        n = MAX_NO_CMD_TO_PROCESS;
        moreCmdsToProcess = true;
    }

    for(rxInt i = 0; i < n; i++){
        Event ev = mCmdQueue->pop();

        string cmd = ev->get(PUSHFYI_MODULE_COMMAND);
        if(cmd.empty()){
            WARN(Log::eTCPServer, "Command parameter not set, Topic = %s Subtopic = %s", ev->getTopic().c_str(), ev->getSubtopic().c_str());
            continue;
        }

        INFO(Log::eTCPServer, "Processing command: %s", cmd.c_str());
        switch(mCommandMap[cmd]) {
			case eStatus:
			{
				idle(0);
			}
			break;

			case eReaderOnline:
			{
				processReaderOnline(ev);
			}
			break;

			default:
			{
                WARN(Log::eTCPServer, "Unknown Command = %s, Topic = %s Subtopic = %s", cmd.c_str(), ev->getTopic().c_str(), ev->getSubtopic().c_str());
			}
			break;
        } //end of switch
    }

    if( moreCmdsToProcess ) {
    	notify();
    }
}

void TCPServer::notify()
{
    //if the write pipe fd is open, write 1 char for select to wake up
    if(mPipeFd[1] > 0) {
        const char* c = "x";
        ::write(mPipeFd[1], c, 1);
        INFO(Log::eTCPServer, "SecureTCPServer command notified.");
    } else {
        ERROR(Log::eTCPServer, "SecureTCPServer notification pipes not open. Command notification failed.");
    }
}

void TCPServer::push(Event event)
{
    submitCmd(event);
}

void TCPServer::pushFront(Event event)
{
    submitCmd(event, true);
}

void TCPServer::submitCmd(Event event, bool isHighPriority)
{
    if (isHighPriority) {
        mCmdQueue->pushFront(event);
    }
    else {
        mCmdQueue->push(event);
    }

    notify();
}

void TCPServer::subscribe()
{
    mRouter.subscribe("", "", this, mTopic, "", -1);
}

void TCPServer::unsubscribe()
{
    mRouter.unsubscribe("", "", this, mTopic);
}

void TCPServer::setAlternatePort(rxInt port)
{
	mIsAlternatePort = true;
	setPort(port);
}

string TCPServer::getUUID()
{
    char   uuidStr[64] = {0};
    uuid_t uuid;

    ::uuid_generate(uuid);
    ::uuid_unparse(uuid, uuidStr);

    return uuidStr;
}

void TCPServer::processReaderOnline(Event& ev)
{
	Synchronized lock(mMutex);

	string reader = ev->get("reader");

	INFO(Log::eTCPReader, "Reader %s is now ready", reader.c_str());
	mReaders.push_back(reader);
}
