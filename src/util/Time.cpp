/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: System timer that runs independently of the
current time. But it must resync after certain interval.
Avoid the use of call gettimeofday every time as it is
a very expensive call.
*********************************************************
*/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Thread.h"
#include "Mutex.h"
#include "Synchronized.h"
#include "Time.h"
//#include "Log.h"

using namespace std;

static Time* theTime;
static struct timeval sTimeval;
static struct timeval sMonotonicTimeval;

static Mutex  sMutex;

RE Time::MSEC_TIMEZONE("([.]([0-9]{3}))?(Z|(([-]|[+])([0-9]{2})[:]?([0-9]{2})?))");

Time* Time::instance()
{
    if(theTime == NULL){
        theTime = new Time();
    }
    return theTime;
}

Time::Time()
    : mThread(NULL), mFinished(false)
{
}

Time::~Time()
{
    if(theTime != NULL){
        mFinished = true;
        if(mThread != NULL){
            mThread->join();
            delete mThread;
        }
        theTime = NULL;
    }
}


rxInt Time::online(rxUInt resolutionInMs, rxUInt resynchInterval)
{
    Synchronized lock(sMutex);

    if(mThread != NULL){
        //DBG(Log::eProcess, "Time already online");
        return -1;
    }

    if(resolutionInMs > MAX_UPDATE_INTERVAL_MS){
        resolutionInMs = MAX_UPDATE_INTERVAL_MS;
    }else if(resolutionInMs < MIN_UPDATE_INTERVAL_MS){
        resolutionInMs = MIN_UPDATE_INTERVAL_MS;
    }

    // correction of about x ms to compensate for sys lag
    mSleepInUsec = resolutionInMs * 1000 - 20000;
    mResynchInterval = resynchInterval;

    ::gettimeofday(&sTimeval, NULL);
    monotonicTime(&sMonotonicTimeval);

    mThread = new Thread(this);
    mThread->start();

    return 0;
}

rxInt Time::offline()
{

    if(mThread != NULL){
        mFinished = true;
        mThread->join();
        delete mThread;
        mThread = NULL;
    }
    return 0;
}



    // get the current time
rxULong Time::time()
{
    timeval tv;

    time(tv);
    return getTimeInMs(tv);
}

rxUInt Time::timeInSeconds()
{
    timeval tv;
    
    time(tv);

    if (tv.tv_usec >= 500000) {
        tv.tv_sec++;
    }

    return tv.tv_sec;
}

    // get the current time
void Time::time(timeval& tv)
{
    if(theTime == NULL){
        ::gettimeofday(&tv, NULL);
    }else{
        Synchronized lock(sMutex);

        tv.tv_sec  = sTimeval.tv_sec;
        tv.tv_usec = sTimeval.tv_usec;
    }
}

    // get the current monotonic time (time since boot)
    // monotonic time is 100 % slower than ::gettimeofday.
    // Can improve efficiency here by quite a bit since we have 
    // a lot of timers in the system.

void Time::getMonotonicTime(timeval& tv)
{
#if defined(_POSIX_MONOTONIC_CLOCK)

    struct timespec ts;

    int ret = ::clock_gettime(CLOCK_MONOTONIC, &ts);
    if (ret != 0) {
        //DBG(Log::eProcess, "Error on system call: %s", strerror(ret));
        Time::time(&tv);
    }else{
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec / 1000;
    }

#else

    Time::time(&tv);

#endif
}

void Time::monotonicTime(timeval& tv)
{
    if(theTime == NULL){
        getMonotonicTime(tv);
    }else{
        Synchronized lock(sMutex);

        tv.tv_sec  = sMonotonicTimeval.tv_sec;
        tv.tv_usec = sMonotonicTimeval.tv_usec;
    }
}

rxULong Time::monotonicTimeInMSecs()
{
    timeval tv;
    rxULong msecs;

    monotonicTime(tv);

    msecs = tv.tv_sec * 1000;

    if (tv.tv_usec >= 500) {
        msecs++;
    }

    return msecs;
}

string Time::timezone()
{
    char      buffer[16];
    string    timezone;
    time_t    now;
    struct tm time;

    ::tzset();
    ::time(&now);
    ::localtime_r(&now, &time);
    ::strftime(buffer, 15, "%z", &time);
    timezone = buffer;
    timezone.insert(3, ":"); // EPCglobal Compliant Format

    return timezone;
}

rxInt Time::timezoneOffset()
{
    char      buffer[16];
    string    timezone;
    rxInt     hours, minutes, offset;
    time_t    now;
    struct tm time;

    ::tzset();
    ::time(&now);
    ::localtime_r(&now, &time);
    ::strftime(buffer, 15, "%z", &time);
    timezone = buffer;
    hours     = ::atoi(timezone.substr(0, 3).c_str());
    minutes   = ::atoi(timezone.substr(4).c_str());
    offset    = (hours * 60 * 60);

    if (offset < 0) {
        offset -= minutes * 60;
    }
    else {
        offset += minutes * 60;
    }

    return offset;
}

rxUInt Time::dayOfWeek(time_t& secs)
{
    struct tm tmTime;

    ::gmtime_r(&secs, &tmTime);

    return tmTime.tm_wday;
}

rxUInt Time::localDayOfWeek(time_t& secs)
{
    struct tm tmTime;

    ::localtime_r(&secs, &tmTime);

    return tmTime.tm_wday;
}

string Time::toString(const string& format, const timeval& tv)
{
    return toString(format, tv.tv_sec);
}

string Time::toString(const string& format, rxUInt secs)
{
    char      buffer[64];
    struct tm time;

    ::gmtime_r((const time_t*) &secs, &time);
    ::strftime(buffer, 63, format.c_str(), &time);

    return buffer;
}

bool Time::toTime(const std::string& timeStr, const std::string& format, time_t& seconds)
{
    struct tm time;

    ::memset(&time, 0, sizeof(struct tm));

    if (::strptime(timeStr.c_str(), format.c_str(), &time) == 0) {
        return false; // FAILED
    }

    errno   = 0;
    seconds = ::timegm(&time);

    if (seconds == -1) {
        if (errno != 0) {
            return false; // FAILED (i.e. mktime returned -1 and errno set)
        }
    }

    return true; // SUCCESS
}

bool Time::toTime(const std::string& timeStr, time_t& seconds)
{
    struct tm time;

    ::memset(&time, 0, sizeof(struct tm));

    const char* remainder = ::strptime(timeStr.c_str(), "%FT%T", &time);

    if (remainder == 0) {
        return false; // FAILED
    }

    errno   = 0;
    seconds = ::timegm(&time);

    if (seconds == -1) {
        if (errno != 0) {
            return false; // FAILED (i.e. mktime returned -1 and errno set)
        }
    }

    string matches[8];

    if (MSEC_TIMEZONE.match(remainder, 8, matches) >= 0) {

        // Round up to nearest second based on specified milliseconds
        if (matches[2].size() > 0) {
            if (::atoi(matches[2].c_str()) >= 500) {
                seconds += 1;
            }
        }

        rxInt adjustment = 0;

        // Hours timezone adjustment
        if (matches[6].size() > 0) {
            adjustment = ::atoi(matches[6].c_str()) * 3600;
        }

        // Minutes timezone adjustment
        if (matches[7].size() > 0) {
            adjustment += ::atoi(matches[7].c_str()) * 60;
        }

        // Sign timezone adjustment
        if (matches[5].size() > 0) {
            if (matches[5][0] == '-') {
                adjustment = -adjustment;
            }
        }

        seconds += adjustment;
    }

    return true; // SUCCESS
}

string Time::localTimeToString(const string& format, timeval& tv)
{
    return localTimeToString(format, tv.tv_sec);
}

string Time::localTimeToString(const string& format, rxUInt secs)
{
    char      buffer[64];
    time_t    seconds = secs;
    struct tm time;

    ::localtime_r(&seconds, &time);
    ::strftime(buffer, 63, format.c_str(), &time);

    return buffer;
}

void Time::resynch()
{
    Synchronized lock(sMutex);

    ::gettimeofday(&sTimeval, NULL);
    getMonotonicTime(sMonotonicTimeval);
}

void Time::run()
{
    rxUInt count = mResynchInterval;
    resynch();

    while(!mFinished){
#ifdef USE_USLEEP
        // the drift with usleep is much worse than using a select
        // at least on Intel - 2.6 kernel
        ::usleep(mSleepInUsec);
#else
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = mSleepInUsec;

        int res = select(0, NULL, NULL, NULL, &tv);
        if(res != 0){
            resynch();
            continue;
        }
#endif
        count--;

        if(count == 0){
            count = mResynchInterval;
            resynch();
        }else{
            sMutex.lock();
            sTimeval.tv_usec += mSleepInUsec;
            if(sTimeval.tv_usec > 1000000){
                sTimeval.tv_sec++;
                sTimeval.tv_usec -= 1000000;
            }
            sMonotonicTimeval.tv_usec += mSleepInUsec;
            if(sMonotonicTimeval.tv_usec > 1000000){
                sMonotonicTimeval.tv_sec++;
                sMonotonicTimeval.tv_usec -= 1000000;
            }
            sMutex.unlock();
        }
    }
}
            
            
        
    

