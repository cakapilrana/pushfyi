/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: System timer that runs independently of the
current time. But it must resync after certain interval.
Avoid the use of call gettimeofday every time as it is
a very expensive call.
*********************************************************
*/
#ifndef __TIME_H__
#define __TIME_H__

#include <string>
#include <unistd.h>
#include <sys/time.h>
#include "pushfyi-types.h"
#include "RE.h"
#include "Runnable.h"

    /**
     * @class Time
     * @ingroup util
     *
     * This class provides a system wide time singleton to avoid calling
     * gettimeofday every time we need the current time value.
     * In this class, we use a thread that sleeps
     * in milliseconds and updates the time value by adding the update
     * interval to the current value. The minimum resolution is 100 ms,
     * max. 500 ms.
     * After the number of resynchIntervals has been expired, gettimeofday
     * will be called to resynchronize the time value with the system time.
     * If future versions allow for a more efficient means of obtaining time,
     * we can reimplement this class without having to change the rest of
     * the code.
     * If online has not been called, the call to time will call gettimeofday
     * every time.
     **/

// The purpose of this class is to

    class Thread;

class Time : public Runnable
{
public:
    static Time* instance();
    virtual ~Time();

    rxInt online(rxUInt updateIntervalInMs = 50, rxUInt resynchInterval = 20);
    rxInt offline();

    // get the current time
    static void time(timeval* tv);
    static void time(timeval& tv);
    static rxULong time();
    static rxUInt timeInSeconds();

    // get the current time since reboot - unaffected by Linux system time changes
    static void monotonicTime(timeval& tv);
    static void monotonicTime(timeval* tv);
    static rxULong monotonicTimeInMSecs();

    static std::string timezone();
    static rxInt timezoneOffset(); // GMT Timezone offset in seconds

    // convert time to formatted string
    static std::string toString(const std::string& format, const timeval& tv);
    static std::string toString(const std::string& format, rxUInt secs);

    // convert ISO8601 formated string to time in seconds since Epoch GMT.
    static bool toTime(const std::string& timeStr, time_t& seconds);

    // convert string to time in seconds since Epoch GMT.
    static bool toTime(const std::string& timeStr, const std::string& format, time_t& seconds);

    static std::string localTimeToString(const std::string& format, timeval& tv);
    static std::string localTimeToString(const std::string& format, rxUInt secs);

    static rxUInt dayOfWeek(time_t& secs);
    static rxUInt localDayOfWeek(time_t& secs);

    static const rxUInt MAX_UPDATE_INTERVAL_MS = 500;
    static const rxUInt MIN_UPDATE_INTERVAL_MS = 50;


private:
    static RE MSEC_TIMEZONE;

    Time();
    virtual void run();
    void         resynch();
    static  void getMonotonicTime(timeval& tv);

    // interval - n * resolutionInMs until call to gettimeofday
    rxUInt     mResynchInterval;

    // resolution in useconds
    useconds_t mSleepInUsec;

    Thread*     mThread;
    bool        mFinished;
};

    // get the current time
inline void Time::time(timeval* tv)
{
    time(*tv);
}

inline void Time::monotonicTime(timeval* tv)
{
    monotonicTime(*tv);
}

    // some helpful time related functions not tied to a particular class
    // but available in namespace rx
inline void setTimeInMs(timeval& tv, rxULong ms)
{
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
}


    // useful for Java interface - note rxULong is a 64bit unsigned long
    // unlike C long which is 32 bits long
inline rxULong getTimeInMs(const timeval& tv)
{
    // processed to avoid overflow
    rxULong val = tv.tv_sec;
    val = val * 1000;
    val += tv.tv_usec / 1000;

    return val;
}

inline void copyTime(timeval& to, const timeval& from)
{
    to.tv_sec = from.tv_sec;
    to.tv_usec = from.tv_usec;
}

inline bool operator ==(timeval& to, const timeval& from)
{
    return ((to.tv_sec == from.tv_sec) && (to.tv_usec == from.tv_usec));
}

inline timeval& operator +=(timeval& to, const timeval& from)
{
    to.tv_usec += from.tv_usec;
    to.tv_sec  += from.tv_sec;

    if(to.tv_usec > 1000000){
        to.tv_sec++;
        to.tv_usec -= 1000000;
    }
    return to;
}

inline timeval& operator -=(timeval& from, const timeval& val)
{
    from.tv_usec -= val.tv_usec;
    from.tv_sec -= val.tv_sec;

    if(from.tv_usec < 0){
        from.tv_sec--;
        from.tv_usec += 1000000;
    }
    return from;
}

inline bool isValidTime(const timeval& tv)
{
    return ((tv.tv_usec >= 0) && (tv.tv_sec >= 0));
}

    // we might want to consider a threshold such as min. time resolution
    // below which the time is considered null
inline bool isNullTime(const timeval& tv)
{
    return ((tv.tv_usec == 0) && (tv.tv_sec == 0));
}

inline void zeroTime(timeval& tv)
{
    tv.tv_sec = 0;
    tv.tv_usec = 0;
}

    // equivalent to left < right
inline bool operator <(const timeval& left, const timeval& right)
{
    if(left.tv_sec < right.tv_sec){
        return true;
    }else if(left.tv_sec > right.tv_sec){
        return false;
    }else{
        return ((left.tv_usec < right.tv_usec)? true : false);
    }
}

    // equivalent to left < right
inline bool operator <=(const timeval& left, const timeval& right)
{
    if(left.tv_sec < right.tv_sec){
        return true;
    }else if(left.tv_sec > right.tv_sec){
        return false;
    }else{
        return ((left.tv_usec <= right.tv_usec)? true : false);
    }
}

    // equivalent to left < right
inline bool operator >(const timeval& left, const timeval& right)
{
    if(left.tv_sec > right.tv_sec){
        return true;
    }else if(left.tv_sec < right.tv_sec){
        return false;
    }else{
        return ((left.tv_usec > right.tv_usec)? true : false);
    }
}

    // equivalent to left <= right
inline bool operator >=(const timeval& left, const timeval& right)
{
    if(left.tv_sec > right.tv_sec){
        return true;
    }else if(left.tv_sec < right.tv_sec){
        return false;
    }else{
        return ((left.tv_usec >= right.tv_usec)? true : false);
    }
}


#endif //__TIME_H__



