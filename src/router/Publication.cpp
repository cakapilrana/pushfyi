#include "Publication.h"
//#include "PubSubRouter.h"
#include "Subscription.h"
#include "PubSubMaps.h"
#include "PubTimerTask.h"
#include "Subscription.h"
#include "Unpublish.h"
#include "Event.h"
#include "SystemTimer.h"
#include "Log.h"
#include "Time.h"

using namespace std;

Publication::Publication(const Event& event, rxUInt ttl)
    : mTopic(event->getTopic()), mSubtopic(event->getSubtopic()), 
      mEvent(event), mRefs(0)
{
    if (!event.isNull()) {
        mTTL = event->getUInt("_RxTTL", ttl);
        mPriority = event->getPriority();
    }
}

Publication::Publication(rxUInt id, const Event& event, rxUInt ttl)
    : PubSubCmd(id, Event::DEFAULT_PRIORITY), mTopic(event->getTopic()), 
      mSubtopic(event->getSubtopic()), mEvent(event), mRefs(0)
{
    if (!event.isNull()) {
        mTTL = event->getUInt("_RxTTL", ttl);
        mPriority = event->getPriority();
    }
}

Publication::~Publication()
{
    for (Subscription* sub = nextSub(); sub != 0; ) {
        Subscription* next = sub->nextSub();

        delete sub;
        sub = next;
    }
}

void Publication::process(PubSubMaps& maps)
{
    maps.publish(this);
}

void Publication::publish(Subscription* sub)
{
    Pushable<Event>* queue    = sub->getPushable();
    string           topic    = mEvent->getTopic();
    string           subtopic = mEvent->getSubtopic();
    string           context  = sub->getContext();
    string           asTopic  = sub->getMappedTopic();

    Event toPublish(mEvent->clone());

    if (asTopic.size() > 0) {
        toPublish->set("topic", topic);
        toPublish->setTopic(asTopic);
    }

/*
    INFO(Log::ePubSub, "Dispatch Event[%s,%s] to Subscriber[%u,%s]", 
          toPublish->getTopic().c_str(), subtopic.c_str(), sub->getId(), context.c_str());
*/

    toPublish->setContext(sub->getContext());
    toPublish->setSubcontext(sub->getSubcontext());

    queue->push(toPublish);

    addSub(sub);
}

void Publication::scheduleTimeout()
{
    SystemTimer* sysTimer = SystemTimer::instance();
    timeval      time, delta;

    setTimeInMs(delta, mTTL);
    Time::monotonicTime(&time);
    time += delta;

    TimerCb timerTask = new PubTimerTaskRep(this, &time);

    sysTimer->add(timerTask);
}

bool Publication::contains(Subscription* toFind)
{
    for (Subscription* sub = nextSub(); sub != 0; sub = sub->nextSub()) {
        if (sub->equalsSubscriber(toFind)) {
            return true; // FOUND
        }
    }

    return false;
}

bool Publication::equals(Publication* toCompare)
{
    return mId == toCompare->mId;
}

bool Publication::equals(Unpublish* toCompare)
{
    return mId == toCompare->getPubId();
}

string Publication::toString()
{
    string result = "P" + StringUtil::toString(getId()) + "(";

    for (Subscription* sub = nextSub(); sub != 0; sub = sub->nextSub()) {
        result += sub->Subscription::toString();

        if (sub->nextSub() != 0) {
            result += ",";
        }
    }

    result += ")";

    return result;
}

