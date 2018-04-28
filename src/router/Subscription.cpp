#include "Subscription.h"
#include "Publication.h"
#include "PubSubMaps.h"
#include "SubTimerTask.h"
#include "Unsubscribe.h"
#include "SystemTimer.h"
#include "Time.h"

using namespace std;

Subscription::Subscription()
    : mPushable(0), mPrev(this), mNext(this)
{
    // nop
}

Subscription::Subscription(Subscription* sub)
    : PubSubCmd(sub), mTopic(sub->mTopic), mSubtopic(sub->mSubtopic), 
      mContext(sub->mContext), mSubcontext(sub->mSubcontext), 
      mPushable(sub->mPushable), mMappedTopic(sub->mMappedTopic), 
      mTimeout(sub->mTimeout), mTimerTask(sub->mTimerTask), 
      mIsMatched(sub->mIsMatched), mPrev(sub->mPrev), mNext(sub->mNext)
{
    // nop
}

Subscription::Subscription(const string&    topic, 
                           const string&    subtopic, 
                           const string&    context, 
                           const string&    subcontext, 
                           Pushable<Event>* pushable, 
                           rxInt            timeout, 
                           const string&    mappedTopic, 
                           rxUInt           priority)
    : mTopic(topic), mSubtopic(subtopic), mContext(context), mSubcontext(subcontext), 
      mPushable(pushable), mMappedTopic(mappedTopic), mTimeout(timeout), 
      mTimerTask(0), mIsMatched(false), mPrev(this), mNext(this)
{
    mPriority = priority;
}

Subscription::~Subscription()
{
    // nop
}

void Subscription::process(PubSubMaps& maps)
{
    maps.subscribe(this);
}

void Subscription::publish(Publication* pub)
{
    mIsMatched = true;

    if (isPersistent()) {
        pub->publish(new Subscription(this));
        reset();
    }
    else {
        pub->publish(this);
    }
}

void Subscription::unsubscribe(Unsubscribe* unsub)
{
    if (!unsub->isTimeout()) {
        removeTimeout();
    }
    else if (!unsub->isProcessed()) {
        Event event = new EventRep("Timeout", mSubtopic);

        event->setContext(mContext);
        event->setSubcontext(mSubcontext);
        event->set("topic",    mTopic);
        event->set("subtopic", mSubtopic);

        /*
         * Experimental: Check added due to Race condition
         *
         * Guard for null check before publishing.
         */
        if(mPushable != 0)
        	mPushable->push(event);

        unsub->processed();
    }
}

void Subscription::scheduleTimeout()
{
    if (mTimeout > 0 && !isMatched() && mTimerTask.isNull()) {
        SystemTimer* sysTimer = SystemTimer::instance();
        timeval      time;

        Time::monotonicTime(&time);
        time.tv_sec += mTimeout;

        mTimerTask = createTimeout(&time);
        sysTimer->add(mTimerTask);
    }
}

void Subscription::scheduleTimeout(Subscription* sub)
{
    scheduleTimeout();
    sub->mTimerTask = mTimerTask;
}

void Subscription::removeTimeout()
{
    if (!mTimerTask.isNull()) {
        SystemTimer* sysTimer = SystemTimer::instance();
        rxUInt       timerId  = mTimerTask->getId();

        mTimerTask = 0;
        sysTimer->cancel(timerId);
    }
}

bool Subscription::contains(Subscription* toFind)
{
    for (Subscription* sub = mNext; sub != this; sub = sub->mNext) {
        if (sub->equals(toFind)) {
            return true; // FOUND
        }
    }

    return false; // NOT FOUND
}

bool Subscription::equals(Subscription* toCompare)
{
    return mTopic      == toCompare->mTopic      && 
           mSubtopic   == toCompare->mSubtopic   && 
           mContext    == toCompare->mContext    && 
           mSubcontext == toCompare->mSubcontext && 
           mPushable   == toCompare->mPushable;
}

bool Subscription::equalsSubscriber(Subscription* toCompare)
{
    return mContext  == toCompare->mContext && 
           mPushable == toCompare->mPushable;
}

SubTimerTaskRep* Subscription::createTimeout(const timeval* expiryTime)
{
    return new SubTimerTaskRep(this, expiryTime);
}

string Subscription::toString()
{
    return "S" + StringUtil::toString(getId());
}
