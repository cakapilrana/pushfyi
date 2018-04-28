/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: A generic queue implementing Pushable interface

*********************************************************
*/
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <string>
#include <vector>

#include "AbsTime.h"
#include "MetricManager.h"
#include "Mutex.h"
#include "Pushable.h"
#include "ConditionVar.h"
#include "Synchronized.h"
#include "Thread.h"
#include "Time.h"
#include "pushfyi-types.h"

template <class T> class Queue
    : public Pushable<T>
{
public:
    Queue(std::string module, std::string submodule, rxSize maxSize)
	: mBuffer(maxSize), mMaxSize(maxSize), mFront(0), mBack(0), mEntryCount(0), mPeak(0), mDelayPeak(0)
    {
        mMetrics    = MetricManager::instance();
        mProcessed  = mMetrics->add(module, submodule + ".queue.processed");
        mFlushed    = mMetrics->add(module, submodule + ".queue.flushed");
        mQueueCount = mMetrics->add(module, submodule + ".queue.count");
        mQueuePeak  = mMetrics->add(module, submodule + ".queue.peak");
        mMaxDelay   = mMetrics->add(module, submodule + ".queue.max-delay");
    }

    virtual ~Queue()
    {
        if (mMetrics != 0) {
            mMetrics->remove(mProcessed);
            mMetrics->remove(mFlushed);
            mMetrics->remove(mQueueCount);
            mMetrics->remove(mQueuePeak);
            mMetrics->remove(mMaxDelay);
        }
    }

    virtual void push(T t)
    {
	Synchronized guard(mMutex);
        rxULong start, duration;

        while (mEntryCount == mMaxSize) { // i.e., full
            start = Time::monotonicTimeInMSecs();
            mNotFull.wait(guard);
            duration = Time::monotonicTimeInMSecs() - start;

            if (duration > mDelayPeak) {
                mDelayPeak = duration;
                mMetrics->setVal(mMaxDelay, mDelayPeak);
            }
	}

	mBuffer[mBack] = t;
	mBack = (mBack + 1) % mMaxSize;
	mEntryCount++;

        mMetrics->setVal(mQueueCount, mEntryCount);

        if (mEntryCount > mPeak) {
            mPeak = mEntryCount;
            mMetrics->setVal(mQueuePeak, mPeak);
        }

        mNotEmpty.notifyAll();
    }

    virtual void pushFront(T t)
    {
	Synchronized guard(mMutex);
        rxULong start, duration;

        while (mEntryCount == mMaxSize) { // i.e., full
            start = Time::monotonicTimeInMSecs();
            mNotFull.wait(guard);
            duration = Time::monotonicTimeInMSecs() - start;

            if (duration > mDelayPeak) {
                mDelayPeak = duration;
                mMetrics->setVal(mMaxDelay, mDelayPeak);
            }
	}

        if (--mFront < 0) {
            mFront = mMaxSize - 1;
        }

	mBuffer[mFront] = t;
	mEntryCount++;

        mMetrics->setVal(mQueueCount, mEntryCount);

        if (mEntryCount > mPeak) {
            mPeak = mEntryCount;
            mMetrics->setVal(mQueuePeak, mPeak);
        }

        mNotEmpty.notifyAll();
    }

    virtual T pop()
    {
	Synchronized guard(mMutex);

        static T sNull(0);

	while (mEntryCount == 0) { // i.e., empty
	    mNotEmpty.wait(guard);
	}

	const T f = mBuffer[mFront];

	// Set the object just read to the null element.
	// This is needed for reference counting.

	mBuffer[mFront] = sNull;
	mFront = (mFront + 1) % mMaxSize;
	mEntryCount--;

        mMetrics->inc(mProcessed);
        mMetrics->setVal(mQueueCount, mEntryCount);

        mNotFull.notifyAll();

	return f;
    }

    virtual void clear()
    {
	Synchronized guard(mMutex);

        static T sNull(0);

        mMetrics->inc(mFlushed, mEntryCount);
        mMetrics->setVal(mQueueCount, 0);

        for (rxInt i = 0; i < mEntryCount; i++) {
            mBuffer[i] = sNull;
        }

        mEntryCount = 0;
        mFront      = 0;
        mBack       = 0;

        mNotFull.notifyAll();
    }

    int getEntryCount()
    {
	return mEntryCount;
    }

    int getMaxSize()
    {
        return mMaxSize;
    }

    int getPeak()
    {
        return mPeak;
    }

    bool isFull()
    {
        return mEntryCount == mMaxSize;
    }

    int waitNotEmpty() const
    {
	Synchronized guard(mMutex);

	while (mEntryCount == 0) { // i.e., empty
	    mNotEmpty.wait(guard);
	}

        return mEntryCount;
    }

    int waitNotEmpty(const rxULong msecs) const
    {
        AbsTime absTime(msecs);
        return waitNotEmpty(absTime);
    }

    int waitNotEmpty(AbsTime& absTime) const
    {
        Synchronized guard(mMutex);

        if (mEntryCount == 0) { // i.e., empty
            mNotEmpty.wait(guard, absTime);
        }

        return mEntryCount;
    }


private:
    std::vector<T>          mBuffer;

    rxInt                   mMaxSize;
    rxInt                   mFront;
    rxInt                   mBack;
    rxInt                   mEntryCount;
    rxInt                   mPeak;
    rxULong                 mDelayPeak;

    mutable ConditionVar    mNotFull;
    mutable ConditionVar    mNotEmpty;
    mutable Mutex           mMutex;

    // Metrics related members
    MetricManager* mMetrics;
    rxUInt         mProcessed;
    rxUInt         mFlushed;
    rxUInt         mQueueCount;
    rxUInt         mQueuePeak;
    rxUInt         mMaxDelay;
};

#endif //__QUEUE_H__
