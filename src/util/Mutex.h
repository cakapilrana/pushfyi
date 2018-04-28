/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: POSIX pthread_mutex_t Wrapper

*********************************************************
*/
#ifndef __MUTEX_H__
#define __MUTEX_H__

#include <pthread.h>

class Mutex
{
public:
    Mutex();
    virtual ~Mutex();

    void lock();
    void unlock();

    pthread_mutex_t* getPthreadMutex();

private:
    pthread_mutex_t mMutex;
};

inline pthread_mutex_t* Mutex::getPthreadMutex()
{
    return &mMutex;
}

#endif //__MUTEX_H__
