/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: POSIX pthread_t wrapper class.

*********************************************************
*/
#include <unistd.h>
#include <stdio.h>
#include "Runnable.h"
#include "Thread.h"

// variable to hold key for the thread-specific Thread object pointer
static pthread_key_t sCurrent_Thread_Key;

// variable to hold once-only initialisation of the sCurrent_Thread_Key key
static pthread_once_t sCurrent_Thread_Key_Once = PTHREAD_ONCE_INIT;

// Allocate the sCurrent_Thread_Key key

static void createCurrentThreadKey()
{
    pthread_key_create(&sCurrent_Thread_Key, 0);
}

static void setCurrentThread(Thread* thread)
{
    // Create the thread-specific key exactly once

    pthread_once(&sCurrent_Thread_Key_Once, createCurrentThreadKey);

    // Set the thread specific data for the key in sCurrent_Thread_Key
    // to the 'this' pointer of this Thread object.

    pthread_setspecific(sCurrent_Thread_Key, thread);
}

static void *startThread(void *arg)
{
    // We are now running in the new thread.
    // We want to set the thread specific data for this thread to
    // point to the C++ Thread object we are associating to this
    // thread. The Thread::currentThread() function uses this thread
    // specific data to get the object pointer.

    Thread *rxThread = (Thread*) arg;

    setCurrentThread(rxThread);
    
    rxThread->run();
    return 0;
}

Thread::Thread()
    : mRunnable(NULL)
{
}

Thread::Thread(Runnable *runnable)
    : mRunnable(runnable)
{
}

Thread::~Thread()
{
}

void Thread::start()
{
    int status = 0;

    status = pthread_create(&mThread, NULL, startThread, this);

    if (status != 0) {
        //LOG(LogId::eThreadStartFailure, Log::eProcess, Log::eError, status);
        return;
    }
}

int Thread::join()
{
    void *status;

    return pthread_join(mThread, &status);
}

void Thread::cancel()
{
    pthread_cancel(mThread);
    pthread_join(mThread, NULL);
}

void Thread::run()
{
    if (mRunnable != NULL) {
	mRunnable->run();
    }
}

void Thread::sleep(long millis)
{
    __useconds_t useconds = (__useconds_t) (millis * 1000);

    usleep(useconds);
}

void Thread::yield()
{
    pthread_yield();
}

Thread* Thread::currentThread()
{
    return (Thread*) pthread_getspecific(sCurrent_Thread_Key);;
}

