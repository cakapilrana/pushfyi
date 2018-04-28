/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: Wrapper for creating critical section code block

*********************************************************
*/
#include "Mutex.h"
#include "Synchronized.h"

Synchronized::Synchronized(Mutex& mutex)
    : mMutex(mutex)
{
    mMutex.lock();
}

Synchronized::~Synchronized()
{
    mMutex.unlock();
}

