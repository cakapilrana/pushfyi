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
#include <string.h>
#include "ClientFactory.h"
#include "PubSubRouter.h"
#include "Log.h"
#include "Properties.h"
#include "ProcessInit.h"
#include "Pushfyi-Defs.h"
#include "TextClient.h"
#include "TLSClient.h"
#include "UnknownClient.h"

#include <iostream>

using namespace std;

ClientFactory* ClientFactory::mInstance = 0;

ClientFactory* ClientFactory::getInstance()
{
    if(0 == mInstance) {
        mInstance = new ClientFactory();
    }
    return mInstance;
}

void ClientFactory::initializeCommandMap()
{
	//mCommandMap[SET_EPOLL_WAIT] = eSetEpollWait;
}

ClientFactory::ClientFactory() : EpollProxy(10), mFinished(false), mThread(0), mTopic(""),
                						mRouter(PubSubRouter::getRouter())
{
    /*
     *  Initialize TCPServer's Command Map
     */
    initializeCommandMap();

    /*
     *  Set the TCPServer's command queue topic
     */
    mTopic          = PUSHFYI_CLIENT_FACTORY_TOPIC;

    /*
     *  Allocate the TCPServer's command queue
     */
    mCmdQueue       = new PriorityQueue<Event>(mTopic, "command", 256);

    /*
     * Create Notification Pipe and Add to EPOLL SET
     */
    if( pipe(mPipeFd) ) {
		//error creating pipe
		ERROR(Log::eClientFactory, "Error creating pipe. TCPServer can not be sent commands.");
    } else {
        //add to EPOLL SET
		if(addEpollFd(mPipeFd[0]) == 0)
            INFO(Log::eClientFactory, "Notification pipe created. Adding fd = %d to select fd set", mPipeFd[0]);
        else
            ERROR(Log::eClientFactory, "Notification pipe failed to add to EpollProxy. TCPreader can not be sent commands.");
    }
}

ClientFactory::~ClientFactory()
{
    //unsubscribe from receiving further messages
    unsubscribe();

    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eClientFactory, "Client Factory Thread stoped");

        //Close notification pipe
        ::close(mPipeFd[0]);
        ::close(mPipeFd[1]);
    }

    //delete the command queue
    if(mCmdQueue != NULL){
        delete mCmdQueue;
    }
}

bool ClientFactory::online()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        INFO(Log::eClientFactory, "Client Factory Thread already running");
        return false;
    }

    subscribe();

    mFinished = false;

    mThread = new Thread(this);
    mThread->start();

    INFO(Log::eClientFactory, "Client Factory Thread started");

    return bRet;
}

bool ClientFactory::offline()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    //unsubscribe from receiving further messages
    unsubscribe();

    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::eClientFactory, "Client Factory Thread stoped");
    }

    return bRet;
}

void ClientFactory::run()
{
    rxInt ret = 0;
	while(!mFinished) {

        /*
         *  It's time to wait for incoming connections
         */
        ret = EpollWait(ClientFactory::DEFAULT_EPOLL_TIME_OUT_MILLIS);
        if( ret == -1 ) {
            INFO(Log::eClientFactory, "error occured on epoll_wait, return = %d", ret);
            //best is to continue for now
            continue;
        }

        if( ret == 0 ) {
            // this is some idle time.
        	idle(ret);
        }

        else if( ret > 0 ) {
            //some events have been triggered
            for(rxInt i=0; i < ret; ++i) {
                epoll_event* ev = getEvent(i);

                //check for error notifications
                if( (ev->events & EPOLLERR) || (ev->events & EPOLLHUP) ||
                		!(ev->events & EPOLLIN)) {

                	if(ev->data.fd == mPipeFd[0]) {
        		        INFO(Log::eClientFactory, "Error occured in notification fd %d", mPipeFd[0]);
                	}
                }

                //check for notification pipe
                if(ev->data.fd == mPipeFd[0]) {
                	INFO(Log::eClientFactory, "Client Factory Received Push FYI Control Command");
                    char buff[10] = {0};
                    //clear the data in the pipe.
                    ::read(mPipeFd[0], buff, sizeof(buff));
                    processCmd();
                }

                else {
                	/*
                	 * if at all there is anything else to do
                	 */
                }
            }
        }
	}
}

void ClientFactory::idle(int retval)
{
	/*
	 * Only touch the clients for whom
	 * session has ended.
	 */
	Synchronized lock(mMutex);

	INFO(Log::eClientFactory, "Client Factory Epoll Timed Out. %d Clients In Cache", mClients.size());

	std::list<PushFYIClient*>::iterator iter;
	for(iter=mClients.begin(); iter!=mClients.end();)
	{
		PushFYIClient* client = *iter;

		if(client->isSessionEnded())
		{
			iter = mClients.erase(iter);
			delete client;
			client = NULL;
		}

		else {
			iter++;
		}
	}
}


void ClientFactory::processCmd()
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
            WARN(Log::eClientFactory, "Command parameter not set, Topic = %s Subtopic = %s", ev->getTopic().c_str(), ev->getSubtopic().c_str());
            continue;
        }

        INFO(Log::eClientFactory, "Processing command: %s", cmd.c_str());
        switch(mCommandMap[cmd]) {
			default:
			{
                WARN(Log::eClientFactory, "Unknown Command = %s, Topic = %s Subtopic = %s", cmd.c_str(), ev->getTopic().c_str(), ev->getSubtopic().c_str());
			}
			break;
        } //end of switch
    }

    if( moreCmdsToProcess ) {
    	notify();
    }
}

void ClientFactory::notify()
{
    //if the write pipe fd is open, write 1 char for select to wake up
    if(mPipeFd[1] > 0) {
        const char* c = "x";
        ::write(mPipeFd[1], c, 1);
        INFO(Log::eClientFactory, "Client Factory command notified.");
    } else {
        ERROR(Log::eClientFactory, "Client Factory notification pipes not open. Command notification failed.");
    }
}

void ClientFactory::push(Event event)
{
    submitCmd(event);
}

void ClientFactory::pushFront(Event event)
{
    submitCmd(event, true);
}

void ClientFactory::submitCmd(Event event, bool isHighPriority)
{
    if (isHighPriority) {
        mCmdQueue->pushFront(event);
    }
    else {
        mCmdQueue->push(event);
    }

    notify();
}

void ClientFactory::subscribe()
{
    mRouter.subscribe("", "", this, mTopic, "", -1);
}

void ClientFactory::unsubscribe()
{
    mRouter.unsubscribe("", "", this, mTopic);
}

PushFYIClient*	ClientFactory::CreateClient(Event& ev)
{
	Synchronized lock(mMutex);

	string ip = ev->get("ip");
	rxUInt port = ev->getUInt("port");
	rxUInt fd = ev->getUInt("fd");
	rxUInt epoll = ev->getUInt("epoll");
	/*
	 * Check the security
	 */
	string security = ev->get("security");

	PushFYIClient* client = 0;

	if(security == "tls") {
		client = new TLSClient(fd, epoll, ip, port);
	}

	else if(security == "text") {
		client = new TextClient(fd, epoll, ip, port);
	}

	else if(security == "unknown") {
		/*
		 * This client may be on alternate port.
		 * Defer client creation
		 */
		client = new UnknownClient(fd, epoll, ip, port);
	}

	mClients.push_back(client);

	return client;
}
