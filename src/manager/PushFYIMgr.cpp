/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS Manager.

This class is responsible for Managing CLient Keys
Validation and maintaining statistics for client
reporting.
*********************************************************
*/
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <curl/curl.h>
#include <openssl/sha.h>
#include "PushFYIMgr.h"
#include "SSLSessionFactory.h"
#include "Log.h"
#include "Properties.h"
#include "ProcessInit.h"


PushFYIManager* PushFYIManager::mInstance = 0;

PushFYIManager* PushFYIManager::getInstance()
{
    if(0 == mInstance) {
        mInstance = new PushFYIManager();
    }
    return mInstance;
}

void PushFYIManager::initializeCommandMap()
{
	/*
	 * Pushfyi Protocol Hello.
	 *
	 * This command validates a client's
	 * publish and subscribe keys.
	 */
    mCommandMap["authorize"] = eAuthorize;
    mCommandMap["authorizecomplete"] = eAuthorizeComplete;
}

PushFYIManager::PushFYIManager() : EpollProxy(10), mThread(0), mFinished(false),
                		mRouter(PubSubRouter::getRouter())
{
    /*
     *  Initialize PushFYIManager's Command Map
     */
    initializeCommandMap();

    /*
     *  Set the TCPServer's command queue topic
     */
    mTopic          = PUSHFYI_MANAGER_TOPIC;

    /*
     *  Allocate the TCPReader's command queue
     */
    mCmdQueue       = new PriorityQueue<Event>(mTopic, "command", 256);

    /*
     * Initialize the fd set and open the notification pipe
     */
    if( pipe(mPipeFd) ) {
		//error creating pipe
		ERROR(Log::ePushFYIManager, "Error creating pipe. TCPreader can not be sent commands.");
    } else {
        //add to EPOLL SET
		if(addEpollFd(mPipeFd[0]) == 0)
            INFO(Log::ePushFYIManager, "Notification pipe created. Adding fd = %d to select fd set", mPipeFd[0]);
        else
            ERROR(Log::ePushFYIManager, "Notification pipe failed to add to EpollProxy. TCPreader can not be sent commands.");
    }
}

PushFYIManager::~PushFYIManager()
{
    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::ePushFYIManager, "PushFYIManager Thread stopped");

        //Close notification pipe
        ::close(mPipeFd[0]);
        ::close(mPipeFd[1]);
    }

    //delete the command queue
    if(mCmdQueue != NULL){
        delete mCmdQueue;
    }
}

bool PushFYIManager::online()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        INFO(Log::ePushFYIManager, "PushFYIManager Thread already running");
        return false;
    }

    subscribe();

    mFinished = false;

    mThread = new Thread(this);
    mThread->start();

    INFO(Log::ePushFYIManager, "PushFYIManager Thread started");

    return bRet;
}

bool PushFYIManager::offline()
{
    Synchronized lock(mMutex);
    bool bRet = true;

    if(mThread != 0) {
        mThread->join();
        delete mThread;
        mThread = 0;
        INFO(Log::ePushFYIManager, "PushFYIManager Thread stopped");
    }

    return bRet;
}

/*
 *  TCPReader Main Listener Thread. All TCP connections must come here.
 *  Client sockets shall than be dispatched to TCPWorker for async io
 */
void PushFYIManager::run()
{
    rxInt ret = 0;

	while(!mFinished) {
        /*
         *  Time to wait for events.
         */
        ret = EpollWait(1000);
        if( ret == -1 ) {
            INFO(Log::ePushFYIManager, "error occured on epoll_wait, return = %d", ret);
            //best is to continue for now
            continue;
        }

        if( ret == 0 ) {
        	/*
        	 * Idle time
        	 */
        	idle(ret);
        }

        else if( ret > 0 ) {
            //some events have been triggered
            for(rxInt i=0; i < ret; i++) {
                epoll_event* ev = getEvent(i);

                //check for error notifications
                if( (ev->events & EPOLLERR) || (ev->events & EPOLLHUP) ||
                		!(ev->events & EPOLLIN)) {

                	if(ev->data.fd == mPipeFd[0]) {
        		        INFO(Log::eSecureTCPServer, "Error occured in notification fd %d", mPipeFd[0]);
                	}

                    removeEpollFd(ev->data.fd);
                }

                //check for notification pipe
                if(ev->data.fd == mPipeFd[0]) {
                    //INFO(Log::ePushFYIManager, "PushFYIManager received pushfyi command.");
                    char buff[10] = {0};
                    //clear the data in the pipe.
                    ::read(mPipeFd[0], buff, sizeof(buff));
                    processCmd();
                }
            } // loop for all triggered events
        } // epoll returned some events

	} // main thread loop
}

void PushFYIManager::processCmd()
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
            WARN(Log::ePushFYIManager, "Command parameter not set, Topic = %s Subtopic = %s", ev->getTopic().c_str(), ev->getSubtopic().c_str());
            continue;
        }

        INFO(Log::ePushFYIManager, "Processing command: %s", cmd.c_str());
        switch(mCommandMap[cmd]) {
			case eAuthorize:
			{
				authorizeMgmtClient(ev);
			}
			break;

			case eAuthorizeComplete:
			{
				authorizeMgmtClientComplete();
			}
			break;

			default:
			{
                WARN(Log::ePushFYIManager, "Unknown Command = %s, Topic = %s Subtopic = %s", cmd.c_str(), ev->getTopic().c_str(), ev->getSubtopic().c_str());
			}
			break;
        } //end of switch
    }

    if( moreCmdsToProcess ) {
    	notify();
    }
}

void PushFYIManager::notify()
{
    //if the write pipe fd is open, write 1 char for select to wake up
    if(mPipeFd[1] > 0) {
        const char* c = "x";
        ::write(mPipeFd[1], c, 1);
        //INFO(Log::ePushFYIManager, "PushFYIManager command notified.");
    } else {
        ERROR(Log::ePushFYIManager, "PushFYIManager notification pipes not open. Command notification failed.");
    }
}

void PushFYIManager::push(Event event)
{
    submitCmd(event);
}

void PushFYIManager::pushFront(Event event)
{
    submitCmd(event, true);
}

void PushFYIManager::submitCmd(Event event, bool isHighPriority)
{
    if (isHighPriority) {
        mCmdQueue->pushFront(event);
    }
    else {
        mCmdQueue->push(event);
    }

    notify();
}

void PushFYIManager::subscribe()
{
    mRouter.subscribe("", "", this, mTopic, "", -1);

    /*
     * Subscribe for Management Client Events
     */
    //mRouter.subscribe("", "", this, PUSHFYI_MGMT_APPSECRET, "", -1);
}

void PushFYIManager::unsubscribe()
{
    mRouter.unsubscribe("", "", this, mTopic);

    /*
     * Unsubscribe for Management Client Events
     */
    //mRouter.unsubscribe("", "", this, PUSHFYI_MGMT_APPSECRET);
}

void PushFYIManager::idle(int)
{
}

void PushFYIManager::authorizeMgmtClient(Event& ev)
{
	Synchronized lock(mMutex);

	string appsecret = ev->get("appsecret");
	string signalingkey = ev->get("signalingkey");

	ev->setTopic(appsecret);
	ev->setSubtopic(signalingkey);

	mRouter.publish(ev);
}

void PushFYIManager::authorizeMgmtClientComplete()
{
	Synchronized lock(mMutex);
}
