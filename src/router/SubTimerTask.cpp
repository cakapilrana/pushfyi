#include "SubTimerTask.h"
#include "AndOrSubscription.h"
#include "Subscription.h"
#include "PubSubRouter.h"
#include "Event.h"
#include "Log.h"

using namespace std;

SubTimerTaskRep::SubTimerTaskRep()
    : mPriority(Event::DEFAULT_PRIORITY)
{
    // nop
}

SubTimerTaskRep::SubTimerTaskRep(Subscription* sub, const timeval* expiryTime)
    : TimerCbRep(expiryTime), mTopics(0), mTopic(sub->getTopic()), 
      mSubtopic(sub->getSubtopic()), mContext(sub->getContext()), 
      mSubcontext(sub->getSubcontext()), mPushable(sub->getPushable()), 
      mPriority(sub->getPriority())
{
    // nop
}

SubTimerTaskRep::SubTimerTaskRep(AndOrSubscription* sub, const timeval* expiryTime)
    : TimerCbRep(expiryTime), mTopics(new vector<string>(*sub->getTopics())), 
      mSubtopic(sub->getSubtopic()), mContext(sub->getContext()), 
      mSubcontext(sub->getSubcontext()), mPushable(sub->getPushable()), 
      mPriority(sub->getPriority())
{
    // nop
}

SubTimerTaskRep::SubTimerTaskRep(SubTimerTaskRep& toCopy)
    : TimerCbRep(toCopy), mTopics(0), mTopic(toCopy.mTopic), mSubtopic(toCopy.mSubtopic), 
      mContext(toCopy.mContext), mSubcontext(toCopy.mSubcontext), mPushable(toCopy.mPushable), 
      mPriority(toCopy.mPriority)
{
    if (toCopy.mTopics != 0) {
        mTopics = new vector<string>(*toCopy.mTopics);
    }
}

SubTimerTaskRep::~SubTimerTaskRep()
{
    if (mTopics != 0) {
        delete mTopics;
    }
}

void SubTimerTaskRep::process(timeval* /* tv */)
{
    PubSubRouter& pubSub = PubSubRouter::getRouter();
    string        topic  = getTopicExpression();


    INFO(Log::ePubSub, "Subscription timeout fired: id=%u, topic=%s, "
        "subtopic=%s, context=%s, subcontext=%s", getId(), topic.c_str(), 
        mSubtopic.c_str(), mContext.c_str(), mSubcontext.c_str());


    if (mTopics == 0) {
        pubSub.unsubscribe(mContext, mSubcontext, mPushable, mTopic, mSubtopic, true, mPriority);
    }
    else {
        pubSub.unsubscribe(mContext, mSubcontext, mPushable, mTopics, mSubtopic, true, mPriority);
    }
}

SubTimerTask::SubTimerTask()
{
    // nop
}

SubTimerTask::SubTimerTask(SubTimerTaskRep* rep)
    : TimerCb(rep)
{
    // nop
}

SubTimerTask::SubTimerTask(const SubTimerTask& toCopy)
    : TimerCb(toCopy)
{
    // nop
}

SubTimerTask::~SubTimerTask()
{
    // nop
}
