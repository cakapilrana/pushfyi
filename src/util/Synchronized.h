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
#ifndef __SYNCHRONIZED_H__
#define __SYNCHRONIZED_H__

class Mutex;

class Synchronized
{
public:
    Synchronized(Mutex& mutex);
    virtual ~Synchronized();

    Mutex& getMutex();

private:
    Mutex& mMutex;
};

inline Mutex& Synchronized::getMutex()
{
    return mMutex;
}

#endif //__SYNCHRONIZED_H__

