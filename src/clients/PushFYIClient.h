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

#ifndef __PUSHFYI_CLIENT__
#define __PUSHFYI_CLIENT__

#include <string>
#include "ClientSocket.h"
#include "Event.h"
#include "Pushable.h"
#include "PriorityQueue.h"
#include "ProtocolHelper.h"
#include "PushFYICodec.h"
#include "PubSubRouter.h"
#include "Log.h"

class PushFYIClient : public ClientSocket, public Pushable<Event> {
public:
	typedef std::map<string, std::list<string>* >	SUBS_MAP;
	typedef std::map<string, std::list<string>* >::iterator	SUBS_MAP_ITER;

	typedef enum __task_status {
		COMPLETED = 0,
		INPROGRESS,
		FAILED
	} TASK_STATUS;

	typedef enum __handshake_status {
		ePushFYIHandshakeReadPending = 0,
		ePushFYIHandshakeWritePending,
		ePushFYIHandshakeDone,
		ePushFYIHandshakeFailed,
		ePushFYIHandshakeTerminated,
		ePushFYISessionEnded
	} HANDSHAKE_STATUS;

public:
	PushFYIClient(rxUInt fd, rxUInt notificationFD, string ip, rxUShort port);
	void operator=(const PushFYIClient&);

	virtual ~PushFYIClient();

	inline void setLicenseKey(string l);
	inline string getLicenseKey();

	inline void setAppSecret(string l);
	inline string getAppSecret();

	inline void setSignalingKey(string l);
	inline string getSignalingKey();

	inline void incrPublishCount();
	inline rxULong getPublishCount();

	/*
	 * Has the handshake completed
	 */
	bool isHandshakeComplete();

	/*
	 * Is the client dead
	 */
	bool isSessionEnded();

	/*
	 * Clients who want to extend the handshake must override this method.
	 *
	 * However, the base class method must be called to complete the handshake.
	 */
	virtual TASK_STATUS doHandshake() = 0;

	/*
	 * Read from the Client
	 */
	virtual TASK_STATUS read(Event&) = 0;

	/*
	 * Write to the client
	 */
	virtual TASK_STATUS write(Event&) = 0;

	/*
	 * Process the out Queue
	 */
	virtual TASK_STATUS process() = 0;

	/*
	 * Ping the client
	 */
	virtual int ping() = 0;

	/*
	 * Receive a Pong
	 */
	virtual int pong();

	/*
	 * Get the security type
	 */
	virtual string getSecurity() = 0;

	/*
	 * Get the protocol type
	 */
	virtual PUSHFYI_PROTOCOL getProtocol();

	/*
	 * Client Subscribe
	 */
    void subscribe(string topic, string subtopic);

    /*
     * Client Unsubscribe
     */
    void unsubscribe(string topic, string subtopic);

    /*
     * Client Publish
     */
    void publish(Event& ev);

    /*
     * Process internal signaling message
     */
    TASK_STATUS processInternalSignal(Event&);

    /*
     * Pushable methods
     */
    void push(Event);
    void pushFront(Event);

    void notify();
    void unnotify();
    //void submitPublication(Event event, bool isHighPriority = false);

    void startHeartBeatTimer(rxULong);

    void expire();

private:
	PushFYIClient(const PushFYIClient&);

protected:

	void submitPublication(Event event, bool isHighPriority = false);

	/*
	 * Read write lock
	 */
	Mutex	mMutex;

	/*
	 * Epoll Instance to Trigger
	 */
    rxUInt  mNotificationFD;

    /*
     * Published Event Queue For This Socket
     */
    PriorityQueue<Event>*	mPublishQueue;

    /*
     * Client's license key
     */
    string	mLicenseKey;

    /*
     * Client's app secret
     */
    string mAppSecret;

    /*
     * Signaling key
     */
    string mSignalingKey;

    /*
     * Publish Counter
     */
    rxULong mPublishCount;

    /*
     * PushFYI Handshake status
     */
    HANDSHAKE_STATUS	mHandshakeStatus;

    /*
     * Protocol of this client
     */
    PUSHFYI_PROTOCOL	mProtocol;

    /*
     * Security scheme of this client
     */
    PUSHFYI_SECURITY	mSecurity;

    /*
     * PushFYI WebSocket Request Header
     */
    PUSHFYI_HEADER		mHeader;

    /*
     * PUSHFYI CODEC
     */
    PushFYICodec* mCodec;

    /*
     * Handshake buffer
     */
    char mHandshakeBuffer[4096*2];

    /*
     * Handshake buffer offset
     */
    rxUInt mHandshakeBufferOffset;

    /*
     * Read pending buffer
     */
    char mReadPending[4096];
    rxUInt mReadPendingOffset;

    char mDecodePending[MAX_PAYLOAD_SIZE];
    rxUInt mDecodePendingOffset;

    char mContinuationData[MAX_PAYLOAD_SIZE];
    rxUInt mContinuationDataOffset;

    /*
     * Subscription list
     */
    SUBS_MAP	mSubs;

    /*
     * Router instance
     */
    PubSubRouter& mRouter;

    /*
     * Timer ID
     */
    rxUInt mTimerId;

    /*
     * PONG Awaited
     */
    bool mPingSent;

    bool mPongReceived;

    rxUInt mHeartbeatsMissed;
};

inline void PushFYIClient::setLicenseKey(string l)
{
	mLicenseKey = l;
}

inline string PushFYIClient::getLicenseKey()
{
	return mLicenseKey;
}

inline void PushFYIClient::setAppSecret(string l)
{
	mAppSecret = l;
}

inline string PushFYIClient::getAppSecret()
{
	return mAppSecret;
}

inline void PushFYIClient::setSignalingKey(string l)
{
	mSignalingKey = l;
}

inline string PushFYIClient::getSignalingKey()
{
	return mSignalingKey;
}

inline void PushFYIClient::incrPublishCount()
{
	++mPublishCount;
}

inline rxULong PushFYIClient::getPublishCount()
{
	return mPublishCount;
}

#endif //__PUSHFYI_CLIENT__
