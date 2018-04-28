#include <iostream>
#include "PubSubRouter.h"
#include "MetricManager.h"
#include "Unsubscribe.h"
#include "AndOrUnsubscribe.h"
#include "Unpublish.h"
#include "PubListener.h"
#include "Log.h"

PubSubRouter* PubSubRouter::mInstance = NULL;

PubSubRouter::PubSubRouter() : mPubListener(0), mThread(0), mFinished(false), mPubCachePeriod(2000)
{
    mQueue = new PriorityQueue<PubSubCmd*>("PubSub", "command", 10240,
			PriorityQueue<PubSubCmd*>::DEFAULT_MAX_PRIORITY,
			new GetPriorityPointer<PubSubCmd*>());

    mMetrics 	= MetricManager::instance();
    mUnsubs     = mMetrics->add("PubSub", "unsubscribed");
    mSubs       = mMetrics->add("PubSub", "subscribed");
    mPubs		= mMetrics->add("PubSub", "published");
    mTimeouts   = mMetrics->add("PubSub", "timeouts");
    mAbsorbed   = mMetrics->add("PubSub", "absorbed");
}

PubSubRouter::~PubSubRouter()
{
    if( mThread ) {
		mFinished = true;
		mThread->join();
		delete mThread;
    }
    delete mQueue;
}

PubSubRouter& PubSubRouter::getRouter()
{
    if( !mInstance ) {
    	mInstance = new PubSubRouter();
    }
    return *mInstance;
}

bool PubSubRouter::online()
{
    if( mThread == 0 ) {
    	mFinished = false;
    	mThread = new Thread(this);
    	mThread->start();
    }
    return true;
}

bool PubSubRouter::offline()
{
    if( mThread ) {
		mFinished = true;
		mQueue->pushFront(0);
		mThread->join();
		delete mThread;
		mThread = 0;
    }
    return true;
}

void PubSubRouter::run()
{
    INFO(Log::eProcess, "PubSub Running.");
    while (!mFinished) {
        PubSubCmd* cmd = mQueue->pop();
        if (cmd != 0) {
            try
            {
                cmd->process(mPubSubMaps);
            }
            catch (...)
            {
            	//LOG(LogId::ePubSubException, Log::ePubSub, Log::eError, "Unknown exception");
            	ERROR(Log::ePubSub, "Unknown PubSub Exception");
            }
        }
    }
//    LOG(LogId::ePubSubOffline, Log::ePubSub, Log::eInfo);

}

rxUInt PubSubRouter::unsubscribe(const string&    context,
                                 const string&    subcontext,
                                 Pushable<Event>* pushable,
                                 const string&    topic,
                                 const string&    subtopic,
                                 bool             isTimeout,
                                 rxUInt           priority)
{
    Unsubscribe* unsub = new Unsubscribe(topic, subtopic, context, subcontext,
                                         pushable, isTimeout, priority);
    rxUInt id = unsub->getId();

    mMetrics->inc(isTimeout ? mTimeouts : mUnsubs);
    mQueue->push(unsub);

    return id;
}

rxUInt PubSubRouter::unsubscribe(const string&    context,
                                 const string&    subcontext,
                                 Pushable<Event>* pushable,
                                 vector<string>*  topics,
                                 const string&    subtopic,
                                 bool             isTimeout,
                                 rxUInt           priority)
{
    AndOrUnsubscribe* unsub = new AndOrUnsubscribe(topics, subtopic, context, subcontext,
                                                   pushable, isTimeout, priority);
    rxUInt id = unsub->getId();

    mMetrics->inc(isTimeout ? mTimeouts : mUnsubs);
    mQueue->push(unsub);

    return id;
}

rxUInt PubSubRouter::unpublish(const string& topic, const string& subtopic, const string& device, rxUInt pubId, rxUInt priority)
{
    Unpublish* unpub = new Unpublish(topic, subtopic, device, pubId, priority);
    rxUInt     id    = unpub->getId();

    mQueue->push(unpub);

    return id;
}

rxUInt PubSubRouter::subscribe(const string&    context,
                               const string&    subcontext,
                               Pushable<Event>* pushable,
                               const string&    topic,
                               const string&    subtopic,
                               rxInt            timeout,
                               const string&    mappedTopic,
                               rxUInt           priority)
{
    Subscription* sub = new Subscription(topic, subtopic, context, subcontext,
                                         pushable, timeout, mappedTopic, priority);
    rxUInt id = sub->getId();

    mMetrics->inc(mSubs);
    mQueue->push(sub);

    return id;
}

rxUInt PubSubRouter::publish(const Event& event)
{
    if (event.isNull()) {
        return 0;
    }

    if (mPubListener != 0 && !mPubListener->publishing(event)) {
        mMetrics->inc(mAbsorbed);
        return 0;
    }

    Publication* pub = new Publication(event, mPubCachePeriod);
    rxUInt       id  = pub->getId();

    mMetrics->inc(mPubs);
    mQueue->push(pub);

    return id;
}


