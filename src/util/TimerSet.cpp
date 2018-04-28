/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Timers container
*********************************************************
*/
#include <iostream>
#include <errno.h>
#include "TimerSet.h"
//#include "Log.h"
#include "Time.h"

using namespace std;

TimerSet::TimerSet()
    : mCount(0)
{
}

TimerSet::TimerSet(const TimerSet& from)
{
    *this = from;
}


TimerSet::~TimerSet()
{
}

TimerSet& TimerSet::operator=(const TimerSet& from)
{
    this->mCount = from.mCount;
    this->mTimerCbs = from.mTimerCbs;

    return *this;
}


rxUInt TimerSet::add(TimerCb& cb)
{
    mCount++;
    cb->setId(mCount);
    mTimerCbs.insert(cb);

    // if it is a named callback, keep track of it in 
    // a map so it can be cancelled by name

    if(cb->hasName()){
        cancel(cb->getName());
        mNameMap.insert(pair<std::string, rxUInt>(cb->getName(), cb->getId()));
        // DBG(Log::eDevice, "Adding name callback: %s, id: %d", cb->getName().c_str(), cb->getId());    
    }

    return mCount;
}

bool TimerSet::cancel(rxUInt id)
{
    set<TimerCb>::iterator p = mTimerCbs.begin();
    set<TimerCb>::iterator end = mTimerCbs.end();

    for(; p != end; p++){
        if((*p)->getId() == id){

            if((*p)->hasName()){
                deleteName((*p)->getName());
            }

            mTimerCbs.erase(p);
            return true;
        }
    }

    return false;
}

bool TimerSet::reset(rxUInt id, const timeval* tv)
{
    set<TimerCb>::iterator p = mTimerCbs.begin();

    for(; p != mTimerCbs.end(); p++){
        if((*p)->getId() == id){
            TimerCb cb = *p;
            mTimerCbs.erase(p);
            cb->setExpiryTime(tv);
            mTimerCbs.insert(cb);
            return true;
        }
    }

    return false;
}

bool TimerSet::cancel(const std::string& name)
{
    rxUInt id;

    if(getIdByName(name, id) == true){
        // DBG(Log::eDevice, "Cb name: %s, id: %u cancelled", name.c_str(), id);
        return cancel(id);
    }else{
        return false;
    }
}

bool TimerSet::getIdByName(const std::string name, rxUInt& id)
{
    NameMapIter it;

    it = mNameMap.find(name);

    if(it != mNameMap.end()){
        id = it->second;
        return true;
    }else{
        return false;
    }
}

bool TimerSet::deleteName(const std::string& name)
{
    NameMapIter it;

    it = mNameMap.find(name);

    if(it != mNameMap.end()){
        mNameMap.erase(it);
        return true;
    }else{
        return false;
    }
}


bool TimerSet::isExpired(const timeval* cbTime, const timeval* expiredTime)
{
    if(cbTime->tv_sec < expiredTime->tv_sec){
        return true;
    }else if( (cbTime->tv_sec == expiredTime->tv_sec)
              && (cbTime->tv_usec < expiredTime->tv_usec) ){
        return true;
    }

    return false;
}
    
rxInt TimerSet::expire(timeval* expiredTime)
{
    rxInt nExpired = 0;
    set<TimerCb>::iterator p = mTimerCbs.begin();
    set<TimerCb>::iterator end = mTimerCbs.end();

    for(; p != end; p++){
        if(isExpired((*p)->getExpiryTime(), expiredTime)){
            if((*p)->hasName()){
                deleteName((*p)->getName());
            }
            (*p)->process(expiredTime);
            nExpired++;
        }else{
            break;
        }
    }

    if(nExpired > 0){
        mTimerCbs.erase(mTimerCbs.begin(), p);
    }

    return nExpired;
}

rxInt TimerSet::expire(timeval* expiredTime, std::vector<TimerCb>& cbs)
{
    rxInt nExpired = 0;
    set<TimerCb>::iterator p = mTimerCbs.begin();
    set<TimerCb>::iterator end = mTimerCbs.end();

    // DBG(Log::eDevice, ">>>> Start:");
    // rxInt it = 1;

    for(; p != end; p++){
        // DBG(Log::eDevice, "Iter: %d", it++);
        if(isExpired((*p)->getExpiryTime(), expiredTime)){

            if((*p)->hasName()){
                deleteName((*p)->getName());
            }
            //(*p)->process(expiredTime);
            cbs.push_back(*p);
            nExpired++;
        }else{
            break;
        }
    }

    if(nExpired > 0){
        mTimerCbs.erase(mTimerCbs.begin(), p);
    }

    return nExpired;
}


void TimerSet::processCallbacks(timeval* expiredTime, std::vector<TimerCb>& cbs)
{
    for(rxUInt i = 0; i < cbs.size(); i++){
        
        cbs[i]->process(expiredTime);

    }
}

const timeval* TimerSet::getNextExpiryTime()
{
    if(mTimerCbs.size() == 0){
        //DBG(Log::eDevice, "No timers set");
        return NULL;
    }
    set<TimerCb>::iterator p = mTimerCbs.begin();

    return (*p)->getExpiryTime();
}

    // get relative time delta 
rxInt TimerSet::getNextTimeDelta(timeval* currentTime, timeval* timeDelta)
{
    const timeval* tv = getNextExpiryTime();

    if(tv == 0){
        return -1;
    }

    timeDelta->tv_sec = tv->tv_sec - currentTime->tv_sec;
    timeDelta->tv_usec = tv->tv_usec - currentTime->tv_usec;

    if (timeDelta->tv_usec < 0) {					      
      --timeDelta->tv_sec;						      
      timeDelta->tv_usec += 1000000;					      
    }									      

    if(timeDelta->tv_sec < 0){
      //DBG(Log::eDevice, "Negative time - should not be possible - please check your logic");
      timeDelta->tv_sec = 0;						      
      timeDelta->tv_usec = 0;
    }

    // make sure we don't have a negative usec
    if(timeDelta->tv_usec < 0){
      timeDelta->tv_usec = 0;
    }

    return 0;
}

    
std::ostream& TimerSet::printString(std::ostream& ostr, rxInt baseSec)
{
    ostr << "TimerSet instance with " << mTimerCbs.size() << " entries" << endl;

    set<TimerCb>::iterator p = mTimerCbs.begin();
    set<TimerCb>::iterator end = mTimerCbs.end();

    for(int i = 0; p != end; p++, i++){
        ostr << i << ": Id:" << (*p)->getId() << " : "  << ((*p)->getExpiryTime()->tv_sec - baseSec) << endl;
    }

    return ostr;
}    


