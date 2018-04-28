/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Timer Callback
*********************************************************
*/
#include <iostream>

#include "TimerCb.h"
//#include "Log.h"
#include "Time.h"

using namespace std;

TimerCbRep::TimerCbRep()
    : mId(0), mCancelled(true)
{
}

TimerCbRep::TimerCbRep(rxUInt deltaExpirySecs)
    : mId(0), mCancelled(false)
{
    timeval time;

    Time::monotonicTime(time);
    time.tv_sec += deltaExpirySecs;
    setExpiryTime(&time);
}

TimerCbRep::TimerCbRep(const second_timer_t /*unused*/, const rxUInt seconds)
    : mId(0), mCancelled(false)
{
    timeval time;
    Time::monotonicTime(time);
    time.tv_sec += seconds;
    setExpiryTime(&time);
}

TimerCbRep::TimerCbRep(const millisecond_timer_t /*unused*/, const rxUInt milliseconds)
    : mId(0), mCancelled(false)
{
    timeval time;
    Time::monotonicTime(time);
    time.tv_usec += milliseconds * 1000;
    time.tv_sec += time.tv_usec / 1000000;
    time.tv_usec %= 1000000;
    setExpiryTime(&time);
}


TimerCbRep::TimerCbRep(const microsecond_timer_t /*unused*/, const rxUInt microseconds)
    : mId(0), mCancelled(false)
{
    timeval time;
    Time::monotonicTime(time);
    time.tv_usec += microseconds;
    time.tv_sec += time.tv_usec / 1000000;
    time.tv_usec %= 1000000;
    setExpiryTime(&time);
}

TimerCbRep::TimerCbRep(rxUInt deltaExpirySecs, const std::string& name)
   : mId(0), mCancelled(false), mName(name)
{
    timeval time;

    Time::monotonicTime(time);
    time.tv_sec += deltaExpirySecs;
    setExpiryTime(&time);
}

TimerCbRep::TimerCbRep(const timeval* expiryTime)
    : mId(0), mCancelled(false)
{
    if(expiryTime != 0){
        setExpiryTime(expiryTime);
    }
}

TimerCbRep::TimerCbRep(TimerCbRep& toCopy)
    : Ref::Rep()
{
    *this = toCopy;
}


TimerCbRep::~TimerCbRep()
{
}


    // the callback function to be called on timeout
void TimerCbRep::process(timeval* tv)
{
//    DBG(Log::eDevice, "Base class instance id: %d time - sec: %d", getId(), tv->tv_sec);
}

TimerCbRep& TimerCbRep::operator=(TimerCbRep& toCopy)
{
    setExpiryTime(toCopy.getExpiryTime());
    mId = toCopy.getId();
    mCancelled = toCopy.isCancelled();

    return *this;
}




