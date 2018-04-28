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
#ifndef _Versum_util_Thread_h
#define _Versum_util_Thread_h

#include <pthread.h>


class Runnable;
    
class Thread
{
public:
    Thread();
    Thread(Runnable *runnable);
    virtual ~Thread();

    static void sleep(long millis);
    static void yield();
    static Thread* currentThread();

    int join();
    void cancel();

    virtual void run();
    void start();

    /* To Be Done:

    static boolean 	interrupted();

    String 	getName();
    void 	setName(String name);
    void 	interrupt() 
    boolean 	isAlive();
    boolean 	isInterrupted();
    void 	join(long millis);
    void 	join(long millis, int nanos);
    */

private:
    pthread_t mThread;
    Runnable *mRunnable;
};


#endif
