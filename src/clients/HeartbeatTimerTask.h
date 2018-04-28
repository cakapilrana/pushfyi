/*
 * HeartbeatTimerTask.h
 *
 *  Created on: 24-Apr-2017
 *      Author: vikas
 */

#ifndef HEARTBEATTIMERTASK_H_
#define HEARTBEATTIMERTASK_H_

#include <string>
#include "Event.h"
#include "TimerCb.h"

class PushFYIClient;

class HeartbeatTimerTaskRep
    : public TimerCbRep
{
public:
	HeartbeatTimerTaskRep();
	HeartbeatTimerTaskRep(PushFYIClient* client, const timeval* expiryTime);
	HeartbeatTimerTaskRep(HeartbeatTimerTaskRep& toCopy);
    virtual ~HeartbeatTimerTaskRep();

    virtual void process(timeval* tv);

private:
    PushFYIClient* mClient;
};

class HeartbeatTimerTask
    : public TimerCb
{
public:
	HeartbeatTimerTask();
	HeartbeatTimerTask(HeartbeatTimerTaskRep* rep);
	HeartbeatTimerTask(const HeartbeatTimerTask& toCopy);
    virtual ~HeartbeatTimerTask();

    HeartbeatTimerTask& operator=(const HeartbeatTimerTask& toCopy);
    bool operator==(const HeartbeatTimerTask& toCompare);

    HeartbeatTimerTaskRep* operator->() const;
    HeartbeatTimerTaskRep* operator*() const;

private:
};

inline HeartbeatTimerTask& HeartbeatTimerTask::operator=(const HeartbeatTimerTask& toCopy)
{
    return (HeartbeatTimerTask&) Ref::operator=(toCopy);
}

inline bool HeartbeatTimerTask::operator==(const HeartbeatTimerTask& toCompare)
{
    return mRep == toCompare.mRep;
}

inline HeartbeatTimerTaskRep* HeartbeatTimerTask::operator->() const
{
    return (HeartbeatTimerTaskRep*) mRep;
}

inline HeartbeatTimerTaskRep* HeartbeatTimerTask::operator*() const
{
    return (HeartbeatTimerTaskRep*) mRep;
}


#endif /* HEARTBEATTIMERTASK_H_ */
