/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS SSL TCP Server Listener.

*********************************************************
*/

#include <sys/epoll.h>
#include <string.h>
#include <algorithm>
#include "PushFYIClient.h"
#include "Pushfyi-Defs.h"
#include "SystemTimer.h"
#include "HeartbeatTimerTask.h"
#include "Time.h"

PushFYIClient::PushFYIClient(rxUInt fd, rxUInt notificationFD,
								string ip, rxUShort port) : ClientSocket(fd, ip, port),
									mNotificationFD(notificationFD), mPublishQueue(0),
									mRouter(PubSubRouter::getRouter()), mCodec(0)
{
    /*
     *  Initialize the publish queue
     */
    mPublishQueue = new PriorityQueue<Event>("pushfyi-client", "command", MAX_CLIENT_QUEUE_SIZE);

    /*
     * Set publish counter = 0
     */
    mPublishCount = 0;

    /*
     * Set the initial handshake status
     */
    mHandshakeStatus = ePushFYIHandshakeReadPending;

    /*
     * Set the Protocol
     */
    mProtocol = eProtocolUnknown;

    /*
     * Set Client's Security Model
     */
    mSecurity = ePushfyiUnknownSecurity;

    /*
     * Set the handshake buffer
     */
    memset(mHandshakeBuffer, 0, sizeof(mHandshakeBuffer));
    mHandshakeBufferOffset = 0;

    /*
     * Clear the sub list
     */
    mSubs.clear();

    mTimerId = 0;

    mPingSent = false;

    mPongReceived = false;

    mHeartbeatsMissed = 0;
}

PushFYIClient::~PushFYIClient()
{
	/*
	 * Delete the publish Queue
	 */
	if(mPublishQueue) {
		delete mPublishQueue;
		mPublishQueue = 0;
	}

	mPublishCount = 0;

    mHandshakeStatus = ePushFYISessionEnded;

    if(mCodec) {
    	delete mCodec;
    	mCodec = 0;
    }
}

void PushFYIClient::operator=(const PushFYIClient& rhs)
{
	INFO(Log::eNetwork, "PushFYIClient Assignment Called");
}

bool PushFYIClient::isHandshakeComplete() {
	return (mHandshakeStatus == ePushFYIHandshakeDone);
}

bool PushFYIClient::isSessionEnded() {
	return (mHandshakeStatus == ePushFYISessionEnded);
}

PUSHFYI_PROTOCOL PushFYIClient::getProtocol()
{
	return mProtocol;
}

void PushFYIClient::push(Event event)
{
    //INFO(Log::eNetwork, "submitting event on fd = %d", getSocket());
    //INFO(Log::eNetwork, "%s", event->toString().c_str());
    submitPublication(event);
}

void PushFYIClient::pushFront(Event event)
{
    submitPublication(event, true);
}

void PushFYIClient::submitPublication(Event event, bool isHighPriority)
{
    if (isHighPriority) {
        mPublishQueue->pushFront(event);
    }
    else {
        mPublishQueue->push(event);
    }

    notify();
}

/*
 * Lets trigger the writter to process the Queue
 */
void PushFYIClient::notify()
{
    /*
     *  Notifiy the mNotificationFD
     *  about the new publication
     */
    struct epoll_event event;
    event.data.ptr = this;

    event.events = EPOLLOUT;

    //event.events = EPOLLOUT;
    rxInt ret = epoll_ctl(mNotificationFD, EPOLL_CTL_MOD, getSocket(), &event);
    if(ret == -1) {
        ERROR(Log::eNetwork, "Failed to notify client on fd = %d for new publications.", getSocket());
    }

    INFO(Log::eNetwork, "Notified client fd = %d of new publishing", getSocket());
}

void PushFYIClient::unnotify()
{
    /*
     *  Notifiy the mNotificationFD
     *  about the new publication
     */
    struct epoll_event event;
    event.data.ptr = this;

    event.events = EPOLLIN;

    //event.events = EPOLLOUT;
    rxInt ret = epoll_ctl(mNotificationFD, EPOLL_CTL_MOD, getSocket(), &event);
    if(ret == -1) {
        ERROR(Log::eNetwork, "Failed to notify client on fd = %d for new publications.", getSocket());
    }

    INFO(Log::eNetwork, "Notified client fd =%d of new publishing", getSocket());
}

void PushFYIClient::subscribe(string topic, string subtopic)
{
	Synchronized lock(mMutex);

	SUBS_MAP_ITER iter = mSubs.find(topic);
	std::list<string>* pList = 0;

	if(iter == mSubs.end())
	{
		pList = new std::list<string>();
		mSubs.insert(std::pair<string, std::list<string>* >(topic, pList));
	}
	else
		pList =	iter->second;

	pList->push_back(subtopic);
	mRouter.subscribe("", "", this, topic, subtopic, -1);
}

void PushFYIClient::unsubscribe(string topic, string subtopic)
{
	Synchronized lock(mMutex);

	SUBS_MAP_ITER iter = mSubs.find(topic);
	std::list<string>* pList = 0;

	if(iter == mSubs.end())
	{
		return;
	}
	else
		pList =	iter->second;

	std::list<string>::iterator it = std::find(pList->begin(), pList->end(), subtopic);
	if(it == pList->end())
		return;
	else {
		pList->remove(subtopic);
		mRouter.unsubscribe("", "", this, topic, subtopic, -1);
	}
}

void PushFYIClient::publish(Event& ev)
{
	mRouter.publish(ev);
}

PushFYIClient::TASK_STATUS PushFYIClient::processInternalSignal(Event& ev)
{
	PushFYIClient::TASK_STATUS ret = COMPLETED;

	if(ev->contains("command"))
	{
		string cmd = ev->get("command");

		if(cmd == "authorize")
		{
			bool status = ev->getBool("status");

			if(status) {
				INFO(Log::eNetwork, "Management Client authorized");
				this->subscribe(getAppSecret(), "__mgmt_notification__");

				ret = COMPLETED;
			}
			else {
				INFO(Log::eNetwork, "Management Client Unauthorized");
				ret = FAILED;
			}
		}

		if(cmd == "kill") {
			ret = FAILED;
		}
	}

	return ret;
}

void PushFYIClient::startHeartBeatTimer(rxULong ms)
{
    SystemTimer* sysTimer = SystemTimer::instance();
    timeval      time, delta;

    setTimeInMs(delta, ms);
    Time::monotonicTime(&time);
    time += delta;

    TimerCb timerTask = new HeartbeatTimerTaskRep(this, &time);

    mTimerId = sysTimer->add(timerTask);
}

void PushFYIClient::expire()
{
	/*
	 * Invalidate all the subscriptions
	 */
	Synchronized lock(mMutex);

	/*
	 * Unsubscribe all of them
	 */
	for(SUBS_MAP_ITER iter = mSubs.begin(); iter != mSubs.end(); iter++)
	{
		string subkey = iter->first;
		std::list<string>* pList = iter->second;

		for(std::list<string>::iterator it = pList->begin(); it != pList->end(); it++) {
			string channel = *it;

			INFO(Log::eNetwork, "Unsubscribing %s, %s", subkey.c_str(), channel.c_str());
			mRouter.unsubscribe("", "", this, subkey, channel);
		}

		delete pList;
	}

	//cancel the heartbeat timer
	SystemTimer* sysTimer = SystemTimer::instance();
	sysTimer->cancel(mTimerId);

	mHandshakeStatus = ePushFYISessionEnded;

	close(getSocket());
}

int PushFYIClient::pong() {
	/*
	 * This is an application layer pong.
	 * Client is alive.
	 */
	mPongReceived = true;
	mPingSent = false;
}
