/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: POSIX pthread_cond_t wrapper

*********************************************************
*/
#include <errno.h>
#include "AbsTime.h"
#include "ConditionVar.h"
#include "Mutex.h"
#include "Synchronized.h"


ConditionVar::ConditionVar()
{
    pthread_cond_init(&mPthrConditionVar, NULL);
}

ConditionVar::~ConditionVar()
{
    // make sure no threads are waiting on this condition variable before destroying it

    notifyAll();
    pthread_cond_destroy(&mPthrConditionVar);
}

void ConditionVar::wait(Synchronized& guard)
{
    Mutex& mutex = guard.getMutex();

    pthread_cond_wait(&mPthrConditionVar, mutex.getPthreadMutex());
}

void ConditionVar::wait(Synchronized& guard, rxULong msecs)
{
    AbsTime absTime(msecs);
    wait(guard, absTime);
}

void ConditionVar::wait(Synchronized& guard, AbsTime& absTime)
{
    Mutex&   mutex = guard.getMutex();

    int rc = pthread_cond_timedwait(&mPthrConditionVar, mutex.getPthreadMutex(), &absTime.get());

    if (rc == ETIMEDOUT)
    {
        absTime.setTimedOut();
    }
}

void ConditionVar::notify()
{
    pthread_cond_signal(&mPthrConditionVar);
}

void ConditionVar::notifyAll()
{
    pthread_cond_broadcast(&mPthrConditionVar);
}

