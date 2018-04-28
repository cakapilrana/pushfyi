/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: AbsTime - Sets the internal clock

*********************************************************
*/
#ifndef __ABSTIME_H__
#define __ABSTIME_H__

#include "pushfyi-types.h"

class AbsTime
{
public:
    AbsTime(const rxULong milliseconds);
    ~AbsTime();

    bool isTimedOut() const;
    const timespec& get() const;
    void set(const timespec& absTime);
    void set(const rxULong milliseconds);
    void setTimedOut();

private:
    bool         mTimedOut;
    timespec     mAbsTime;
};

inline AbsTime::AbsTime(const rxULong milliseconds)
{
    set(milliseconds);
}

inline AbsTime::~AbsTime()
{
    // nop
}

inline bool AbsTime::isTimedOut() const
{
    return mTimedOut;
}

inline const timespec& AbsTime::get() const
{
    return mAbsTime;
}

inline void AbsTime::setTimedOut()
{
    mTimedOut = true;
}

inline void AbsTime::set(const timespec& absTime)
{
    mTimedOut = false;
    mAbsTime.tv_sec  = absTime.tv_sec;
    mAbsTime.tv_nsec = absTime.tv_nsec;
}

inline void AbsTime::set(const rxULong milliseconds)
{
    timespec absTime;

    ::clock_gettime(CLOCK_REALTIME, &absTime);

    absTime.tv_sec  += milliseconds / 1000L;
    absTime.tv_nsec += (milliseconds % 1000L) * 1000000L;
    if (absTime.tv_nsec > 1000000000L)
    {
        absTime.tv_sec  += 1;
        absTime.tv_nsec %= 1000000000L;
    }

    set(absTime);
}

#endif //__ABSTIME_H__
