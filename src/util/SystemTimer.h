/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Internal System Timer for timing out Events.

*********************************************************
*/
#ifndef __SYSTEMTIMER_H__
#define __SYSTEMTIMER_H__

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "ConditionVar.h"
#include "Mutex.h"
#include "Runnable.h"
#include "TimerSet.h"


class Thread;

class SystemTimer
    : public Runnable
{
public:
    static SystemTimer* instance();
    virtual ~SystemTimer();

    bool online();
    bool offline();

    // add timer callback - returns a unique id that can be used for cancel
    // also sets id internally in the TimerCb object
    rxUInt add(TimerCb& cb);

    // add timer callback - returns a unique id that can be used for cancel
    // Sets the name used for cancel by name cbs
    // also sets id and name  internally in the TimerCb object
    rxUInt add(TimerCb& cb, const std::string& name);
    
    // cancel by using the unique timer id returned by add
    bool cancel(rxUInt id);

    // cancel by using the name used by add
    bool cancel(const std::string& name);

    bool reset(rxUInt id, rxUInt milliseconds);

    bool reset(rxUInt id, const timeval* tv);

    void notify();

    rxInt size();

    // remove all callbacks entries
    void clear();

    std::ostream& printString(std::ostream& ostr);

private:
    SystemTimer();

    void addSelectFd(rxInt fd);
    void removeSelectFd(rxInt fd);
    timeval* expireCbs(timeval* runTimeDelta);

    virtual void run(); // runs select loop for input from devices

    static SystemTimer* mInstance;

    Mutex          mMutex;
    Thread*        mThread;
    bool           mFinished;

    fd_set         mReadFdSet;
    int            mHighestFd;
    int            mPipeFd[2];
    rxInt          mInterrupted;

    TimerSet       mTimerSet;
};

inline rxInt SystemTimer::size()
{
    return mTimerSet.size();
}

inline void SystemTimer::clear()
{
    mTimerSet.clear();
}

#endif //__SYSTEMTIMER_H__
