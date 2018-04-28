#ifndef __SUBTIMER_TASK_H__
#define __SUBTIMER_TASK_H__

#include <string>
#include "Event.h"
#include "Pushable.h"
#include "TimerCb.h"

class Subscription;
class AndOrSubscription;

class SubTimerTaskRep
    : public TimerCbRep
{
public:
    SubTimerTaskRep();
    SubTimerTaskRep(Subscription* sub, const timeval* expiryTime);
    SubTimerTaskRep(AndOrSubscription* sub, const timeval* expiryTime);
    SubTimerTaskRep(SubTimerTaskRep& toCopy);
    virtual ~SubTimerTaskRep();

    virtual void process(timeval* tv);

private:
    std::string getTopicExpression();

    std::vector<std::string>* mTopics;
    std::string               mTopic;
    std::string               mSubtopic;
    std::string               mContext;
    std::string               mSubcontext;
    Pushable<Event>*          mPushable;
    rxUInt                    mPriority;
};

inline std::string SubTimerTaskRep::getTopicExpression()
{
    if (mTopics == 0) {
        return mTopic;
    }

    std::vector<std::string>& topics = *mTopics;
    rxUInt                    length = topics.size();
    std::string               result;

    for (rxUInt i = 0; i < length; i++) {
        result += topics[i];

        if (i + 1 < length) {
            result += "|";
        }
    }

    return result;
}

class SubTimerTask
    : public TimerCb
{
public:
    SubTimerTask();
    SubTimerTask(SubTimerTaskRep* rep);
    SubTimerTask(const SubTimerTask& toCopy);
    virtual ~SubTimerTask();

    SubTimerTask& operator=(const SubTimerTask& toCopy);
    bool operator==(const SubTimerTask& toCompare);

    SubTimerTaskRep* operator->() const;
    SubTimerTaskRep* operator*() const;

private:
};

inline SubTimerTask& SubTimerTask::operator=(const SubTimerTask& toCopy)
{
    return (SubTimerTask&) Ref::operator=(toCopy);
}

inline bool SubTimerTask::operator==(const SubTimerTask& toCompare)
{
    return mRep == toCompare.mRep;
}

inline SubTimerTaskRep* SubTimerTask::operator->() const
{
    return (SubTimerTaskRep*) mRep;
}

inline SubTimerTaskRep* SubTimerTask::operator*() const
{
    return (SubTimerTaskRep*) mRep;
}

#endif //__SUBTIMER_TASK_H__
