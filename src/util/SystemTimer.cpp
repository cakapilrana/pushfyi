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
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <vector>
#include <string.h>
//#include "Log.h"
#include "Thread.h"
#include "TimerCb.h"
#include "SystemTimer.h"
#include "Time.h"

using namespace std;

SystemTimer* SystemTimer::mInstance = 0;

SystemTimer* SystemTimer::instance()
{
    if (mInstance == 0) {
        mInstance = new SystemTimer();
    }
    
    return mInstance;
}

SystemTimer::SystemTimer()
    : mThread(NULL), mFinished(false), mInterrupted(0)
{
    /* Watch if pipe has input from notify */
    FD_ZERO(&mReadFdSet);

    if(pipe(mPipeFd)){
        //DBG(Log::eProcess, "call to syscall pipe failed used for Notification: Reason: %s", strerror(errno));
    }else{
        addSelectFd(mPipeFd[0]);
    }
}

SystemTimer::~SystemTimer()
{
    // notify the worker thread and wait for it to finish
    if(mThread){
        mFinished = true;
        mInterrupted = true;
        notify();  
        mThread->join();

        delete mThread;
        close(mPipeFd[0]);
        close(mPipeFd[1]);
    }
}

bool SystemTimer::online()
{
    Synchronized lock(mMutex);

    if(mPipeFd[0] == -1){
        //DBGFINEST(Log::eProcess, "System timer does not have notification pipe - aborting online");
        return false;
    }

    if (mThread == 0) {
        mFinished = false;

        mThread   = new Thread(this);
        mThread->start();

        //DBG(Log::eProcess, "SystemTimer online");
    }else{
        //DBG(Log::eProcess, "SystemTimer already online - need to go offline first");
    }
    return true;
}

bool SystemTimer::offline()
{
    //Synchronized lock(mMutex);

    // notify the worker thread and wait for it to finish
    if(mThread){
        mFinished = true;
        mInterrupted = true;
        notify();  
        mThread->join();

        delete mThread;
        mThread = NULL;
    }

    return true;
}

void SystemTimer::notify()
{
    // if the write pipe fd is open, write 1 char for select to wake up
    if(mPipeFd[1] > 0){ 
        const char* c = "x";
        ::write(mPipeFd[1], c, 1);
        //DBGFINEST(Log::eProcess, "SystemTimer notification executed");
    }else{
        //DBG(Log::eProcess, "ERROR::SystemTimer notification pipes not open");
    }
}

void SystemTimer::addSelectFd(rxInt fd)
{
    if(fd < 0){
        //DBG(Log::eProcess, "Invalid file descriptor: %d", fd);
    }else{
        FD_SET(fd, &mReadFdSet);
    }
}

void SystemTimer::removeSelectFd(rxInt fd)
{
    if(fd < 0){
        //DBG(Log::eProcess, "Invalid file descriptor: %d", fd);
    }else{
        FD_CLR(fd, &mReadFdSet); 
    }
}

rxUInt SystemTimer::add(TimerCb& cb)
{
    Synchronized lock(mMutex);
    rxUInt rc = mTimerSet.add(cb);
    notify();
    return rc;
}

rxUInt SystemTimer::add(TimerCb& cb, const std::string& name)
{
    cb->setName(name);
    return add(cb);
}


bool SystemTimer::cancel(rxUInt id)
{
    Synchronized lock(mMutex);
    bool rc = mTimerSet.cancel(id);
    notify();
    return rc;
}
    
bool SystemTimer::reset(rxUInt id, rxUInt milliseconds)
{
    timeval time;
    Time::monotonicTime(time);
    time.tv_usec += milliseconds * 1000;
    time.tv_sec += time.tv_usec / 1000000;
    time.tv_usec %= 1000000;
    return reset(id, &time);
}
    
bool SystemTimer::reset(rxUInt id, const timeval* tv)
{
    Synchronized lock(mMutex);
    bool rc = mTimerSet.reset(id, tv);
    notify();
    return rc;
}
    
bool SystemTimer::cancel(const std::string& name)
{
    Synchronized lock(mMutex);
    bool rc = mTimerSet.cancel(name);
    notify();
    return rc;
}
    

// if there is not callbacks to be expired, we just block
// on the select. This means a NULL timeval pointer is returned here.

timeval* SystemTimer::expireCbs(timeval* runTimeDelta)
{
    mMutex.lock();

    if(size() == 0){
        mMutex.unlock();
        return NULL;
    }

    timeval current;
    Time::monotonicTime(&current);

    vector<TimerCb> cbs;

    rxInt n = mTimerSet.expire(&current, cbs);

    //DBGFINEST(Log::eProcess, "Expired %d callbacks", n);

    if(size() == 0){

        runTimeDelta = NULL;

    }else{

        if(mTimerSet.getNextTimeDelta(&current, runTimeDelta) == -1){
            //DBG(Log::eProcess, "Unable to determine next expiry time delta");
            runTimeDelta = NULL;
        }
    }

    // unlock the mutex and fire the cbs
    mMutex.unlock();

    mTimerSet.processCallbacks(&current, cbs);

    return runTimeDelta;
}

    
    

void SystemTimer::run()
{
    timeval tv;
    /* Watch if pipe has input from notify */
    fd_set fdSet;
    int retval;
    timeval* expireTimeDelta = expireCbs(&tv);

    while(!mFinished){
        // copy the current file descriptor set 
        memcpy(&fdSet, &mReadFdSet, sizeof(fdSet));

        // if the expire delta is NULL, it blocks forever
        // adding a cb will notify this loop and then set the expireTime on the next round
        retval = select(mPipeFd[0] + 1, &fdSet, NULL, NULL, expireTimeDelta);

        if (retval == -1){
            //DBG(Log::eProcess, "Select return -1 - reason: %s", strerror(errno));
            if(errno != EAGAIN && errno != EINTR){
                //DBG(Log::eProcess, "Timer loop error");

                // see if we we get the Timer loop error repeatedly - may need to implement
                // exit here.
            }
        }else if (retval){
            // check if we have been notified
            if(FD_ISSET(mPipeFd[0], &fdSet)){
                //DBGFINEST(Log::eProcess, "Notification");
                char buff[8096] = {0};
                ::read(mPipeFd[0], buff, sizeof(buff)); // clear out the data in the pipe
                expireTimeDelta = expireCbs(&tv);
            }
        } else { // received timeout - retval == 0
            //DBGFINEST(Log::eProcess, "Timeout triggered");
            expireTimeDelta = expireCbs(&tv);
        }

    }
}

