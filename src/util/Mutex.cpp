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
#include <unistd.h>
#include <stdio.h>
#include "Mutex.h"

Mutex::Mutex()
{
    pthread_mutexattr_t mutexattr;

    pthread_mutexattr_init(&mutexattr);

    // PTHREAD_MUTEX_RECURSIVE_NP is non-portable but very convenient for us

    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&mMutex, &mutexattr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&mMutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&mMutex);
}

