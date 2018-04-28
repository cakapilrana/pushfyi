#ifndef __PUBTIMER_TASK_H__
#define __PUBTIMER_TASK_H__

#include <string>
#include "Event.h"
#include "TimerCb.h"

class Publication;

class PubTimerTaskRep
    : public TimerCbRep
{
public:
    PubTimerTaskRep();
    PubTimerTaskRep(Publication* pub, const timeval* expiryTime);
    PubTimerTaskRep(PubTimerTaskRep& toCopy);
    virtual ~PubTimerTaskRep();

    virtual void process(timeval* tv);

private:
    rxUInt      mPubId;
    std::string mTopic;
    std::string mSubtopic;
    std::string mDevice;
    rxUInt      mPriority;
};

class PubTimerTask
    : public TimerCb
{
public:
    PubTimerTask();
    PubTimerTask(PubTimerTaskRep* rep);
    PubTimerTask(const PubTimerTask& toCopy);
    virtual ~PubTimerTask();

    PubTimerTask& operator=(const PubTimerTask& toCopy);
    bool operator==(const PubTimerTask& toCompare);

    PubTimerTaskRep* operator->() const;
    PubTimerTaskRep* operator*() const;

private:
};

inline PubTimerTask& PubTimerTask::operator=(const PubTimerTask& toCopy)
{
    return (PubTimerTask&) Ref::operator=(toCopy);
}

inline bool PubTimerTask::operator==(const PubTimerTask& toCompare)
{
    return mRep == toCompare.mRep;
}

inline PubTimerTaskRep* PubTimerTask::operator->() const
{
    return (PubTimerTaskRep*) mRep;
}

inline PubTimerTaskRep* PubTimerTask::operator*() const
{
    return (PubTimerTaskRep*) mRep;
}

#endif //__PUBTIMER_TASK_H__
