/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: POSIX pthread_cond_t wrapper

*********************************************************
*/
#ifndef __CONDITION_VAR_H__
#define __CONDITION_VAR_H__

#include <pthread.h>
#include "pushfyi-types.h"

class Synchronized;
class AbsTime;

class ConditionVar
{
public:
    ConditionVar();
    virtual ~ConditionVar();

    virtual void wait(Synchronized& guard);
    virtual void wait(Synchronized& guard, rxULong msecs);
    virtual void wait(Synchronized& guard, AbsTime& absTime);

    virtual void notify();
    virtual void notifyAll();

private:
    pthread_cond_t mPthrConditionVar;
};

#endif //__CONDITION_VAR_H__
