/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Timers container
*********************************************************
*/
#ifndef __TIMER_SET_H__
#define __TIMER_SET_H__

#include <sys/time.h>
#include <vector>
#include <set>
#include <map>
#include <string>
#include "pushfyi-types.h"
#include "TimerCb.h"
#include "STLHelpers.h"

// Class to be used in a single thread. This class is also used by
// the system timer singleton.

class TimerSet
{
public:

    TimerSet();
    TimerSet(const TimerSet& from);

    virtual ~TimerSet();

    TimerSet& operator=(const TimerSet &from);
    bool operator==(const TimerSet &from);
    bool operator!=(const TimerSet &from);

    // add timer callback - returns a unique id
    rxUInt add(TimerCb& cb);

    // get id by name
    // @return - true if cbs exists - id contains cb id, otherwise false
    bool getIdByName(const std::string name, rxUInt& id);

    // cancel by using the unique timer id returned by add
    bool cancel(rxUInt id);

    // cancel by name, requires that the name of the cb was set when added originally
    // @return - true on success, false if name does not exist (maybe already expired)
    bool cancel(const std::string& name);

    // reset timer
    bool reset(rxUInt id, const timeval* tv);

    // Expired callbacks are removed processed and from the set
    // It is up to the client to reschedule it.
    // Expiry time is absolute not relative
    rxInt expire(timeval* tv);

    // Expired callbacks are removed from the set and put into cbs vector
    // Unlike the version above, it will not execute the callback process functions
    // so it can be used in a synchronized call block.
    // It is up to the client to reschedule it.
    // Expiry time is absolute not relative
    // It is up to the client to execute the Callback process function
    rxInt expire(timeval* tv, std::vector<TimerCb>& cbs);

    // Call the process function on the passed in array of cbs
    void processCallbacks(timeval* expiredTime, std::vector<TimerCb>& cbs);

    // get next absolute expiry time
    const timeval* getNextExpiryTime();

    // get relative time delta
    rxInt getNextTimeDelta(timeval* currentTime, timeval* timeDelta);

    rxInt size();

    // remove all callbacks and name entries
    void clear();

    std::ostream& printString(std::ostream& ostr, rxInt baseSec);

private:
    //typedef __gnu_cxx::hash_map<std::string, rxUInt, StringHash, StringEqual> NameMap;
    typedef std::map<std::string, rxUInt> NameMap;
    typedef NameMap::iterator NameMapIter;

    bool isExpired(const timeval* cbTime, const timeval* expiredTime);
    bool deleteName(const std::string& name);

    // use a set for an efficient timer callback implementation
    // Once we run metrics, we may change the implementation to something else
    // if it is not fast enough
    rxUInt            mCount;
    std::set<TimerCb> mTimerCbs;
    timeval           mTimeDelta;  // relative time to next event

    NameMap           mNameMap;   //  contains named cbs entries
};


inline rxInt TimerSet::size()
{
    return mTimerCbs.size();
}

inline void TimerSet::clear()
{
    mTimerCbs.clear();
    mNameMap.clear();
}

#endif // __TIMER_SET_H__



