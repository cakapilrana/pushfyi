#ifndef __PUBSUB_ROUTER_H__
#define __PUBSUB_ROUTER_H__

#include "Runnable.h"
#include "Thread.h"
#include "PriorityQueue.h"
#include "PubSubMaps.h"

class PubSubCmd;
class MetricManager;
class PubListener;

class PubSubRouter : public Runnable {

public:
    static PubSubRouter& getRouter();
    ~PubSubRouter();

    void run();
    bool online();
    bool offline();


private:
    PubSubRouter();
    PubSubRouter(const PubSubRouter&);

    friend class PubSubEntry;

    PubSubMaps			mPubSubMaps;
    PriorityQueue<PubSubCmd*>*	mQueue;
    MetricManager*		mMetrics;
    PubListener*		mPubListener;

    static PubSubRouter* mInstance;
    Thread*		mThread;
    bool  		mFinished; 
    rxUInt         	mUnsubs;
    rxUInt		mSubs;
    rxUInt		mPubs;
    rxUInt		mAbsorbed;
    rxUInt         	mTimeouts;
    rxUInt		mPubCachePeriod;

public:

    void addPubListener(PubListener*);

    rxUInt unsubscribe(const std::string& context,
                       const std::string& subcontext,
                       Pushable<Event>*   pushable,
                       const std::string& topic,
                       const std::string& subtopic  = RX_ESREF,
                       bool               isTimeout = false,
                       rxUInt             priority  = Event::DEFAULT_PRIORITY);

    rxUInt unsubscribe(const std::string&        context,
                       const std::string&        subcontext,
                       Pushable<Event>*          pushable,
                       std::vector<std::string>* topics,
                       const std::string&        subtopic  = RX_ESREF,
                       bool                      isTimeout = false,
                       rxUInt                    priority    = Event::DEFAULT_PRIORITY);

    rxUInt unpublish(const std::string& 	topic, 
		       	const std::string& 	subtopic, 
			const std::string& 	device, 
			rxUInt 			pubId, 
			rxUInt 			priority = Event::DEFAULT_PRIORITY);

    rxUInt subscribe(const std::string& context,
                     const std::string& subcontext,
                     Pushable<Event>*   pushable,
                     const std::string& topic,
                     const std::string& subtopic    = RX_ESREF,
                     rxInt              timeout     = 0,
                     const std::string& mappedTopic = RX_ESREF,
                     rxUInt             priority    = Event::DEFAULT_PRIORITY);

    rxUInt publish(const Event& event);


};

inline void PubSubRouter::addPubListener(PubListener* pubListener)
{
    mPubListener = pubListener;
}


#endif //__PUBSUB_ROUTER_H__
