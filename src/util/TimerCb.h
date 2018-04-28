/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Timer Callback
*********************************************************
*/
#ifndef  __TIMERCB_H__
#define  __TIMERCB_H__

#include <sys/time.h>
#include "pushfyi-types.h"
#include "Ref.h"

class TimerCbRep;

class TimerCb : public Ref
{
public:
    TimerCb() : Ref()
    {
    }

    TimerCb(Ref::Rep* rep)
	: Ref(rep)
    {
    }

    TimerCb(const TimerCb& toCopy)
	: Ref(toCopy)
    {
    }

    virtual ~TimerCb()
    {
    }

    TimerCb& operator=(const TimerCb& toCopy)
    {
        return (TimerCb&) Ref::operator=(toCopy);
    }

    bool operator==(const TimerCb& toCompare)
    {
        return mRep == toCompare.mRep;
    }

    TimerCbRep* operator->() const
    {
	return (TimerCbRep*) mRep;
    }

    TimerCbRep* operator*() const
    {
	return (TimerCbRep*) mRep;
    }
};

class TimerCbRep
    : public Ref::Rep
{
public:
    // Type markers -- no need to define
    // Usage: e.g. TimerCb(new TimerCbRep(millisecond_timer, 50)); // 50ms
    static struct second_timer_t {} second_timer;
    static struct millisecond_timer_t {} millisecond_timer;
    static struct microsecond_timer_t {} microsecond_timer;

    TimerCbRep();
    TimerCbRep(rxUInt deltaExpirySecs);
    TimerCbRep(const second_timer_t, const rxUInt seconds);
    TimerCbRep(const millisecond_timer_t, const rxUInt milliseconds);
    TimerCbRep(const microsecond_timer_t, const rxUInt microseconds);
    TimerCbRep(rxUInt deltaExpirySecs, const std::string& name);
    TimerCbRep(const timeval* expiryTime);
    TimerCbRep(TimerCbRep& toCopy);

    virtual ~TimerCbRep();

    // the callback function to be called on timeout
    virtual void process(timeval* tv);

    TimerCbRep& operator=(TimerCbRep& toCopy);
    bool        operator==(TimerCbRep& other);
    bool        operator!=(TimerCbRep& toCopy);

    const timeval* getExpiryTime();
    void setExpiryTime(const timeval* tv);

    std::string getName();
    void        setName(const std::string& name);
    bool        hasName();

    rxUInt getId();
    void   setId(rxUInt id);

    bool isCancelled();
    void setCancelled(bool b);

private:
    rxUInt      mId;
    timeval     mExpiryTime;
    bool        mCancelled;
    std::string mName;
};


inline const timeval* TimerCbRep::getExpiryTime()
{
    return &mExpiryTime;
}


inline  void TimerCbRep::setExpiryTime(const timeval* tv)
{
    mExpiryTime.tv_sec = tv->tv_sec;
    mExpiryTime.tv_usec = tv->tv_usec;
}

inline bool TimerCbRep::operator==(TimerCbRep& other)
{
    return ((getExpiryTime() == other.getExpiryTime()) && (getId() == other.getId()));
}

inline std::string TimerCbRep::getName()
{
    return mName;
}

inline void TimerCbRep::setName(const std::string& name)
{
    mName = name;
}

inline bool TimerCbRep::hasName()
{
    return !mName.empty();
}

inline bool TimerCbRep::operator!=(TimerCbRep& other)
{
    return !(*this == other);
}


    // operator for set
inline bool operator<(const TimerCb& a, const TimerCb& b)
{
    const timeval* left = a->getExpiryTime();
    const timeval* right = b->getExpiryTime();

    if(left->tv_sec < right->tv_sec){
        return true;
    }else if(left->tv_sec > right->tv_sec){
        return false;
    }else if (left->tv_usec < right->tv_usec) {  // seconds are equal
        return true;
    }else if (left->tv_usec > right->tv_usec) {  // seconds are equal
        return false;
    }
    else {  // seconds & useconds are equal
        return (a->getId() < b->getId());
    }
}

inline  rxUInt TimerCbRep::getId()
{
    return mId;
}

inline  void TimerCbRep::setId(rxUInt id)
{
    mId = id;
}

inline bool TimerCbRep::isCancelled()
{
    return mCancelled;
}

inline void TimerCbRep::setCancelled(bool b)
{
    mCancelled = b;
}

#endif // __TIMERCB_H__
