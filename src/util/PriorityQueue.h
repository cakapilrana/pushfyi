/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: PriorityQueue - Template priority queue designed
for Event Objects. May also be used for other types.

*********************************************************
*/
#ifndef __PRIORITY_QUEUE_H__
#define __PRIORITY_QUEUE_H__

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
#include "STLHelpers.h"

template <class T> class GetPriority
{
public:
    GetPriority() {};
    virtual ~GetPriority() {};

    virtual rxUInt getPriority(T t, rxUInt maxPriority) = 0;
};

template <class T> class GetPriorityRefCounted
    : public GetPriority<T>
{
    virtual rxUInt getPriority(T t, rxUInt maxPriority)
    {
        if (t.isNull() || t->getPriority() >= maxPriority) {
            return maxPriority - 1;
        }

        return t->getPriority();
    }
};

template <class T> class GetPriorityPointer
    : public GetPriority<T>
{
    virtual rxUInt getPriority(T t, rxUInt maxPriority)
    {
        if (t == 0 || t->getPriority() >= maxPriority) {
            return maxPriority - 1;
        }

        return t->getPriority();
    }
};

/**
 * @ingroup util
 */
template <class T> class PriorityQueue
    : public Pushable<T>
{
public:
    static const rxUInt DEFAULT_MAX_PRIORITY = 4;

    PriorityQueue(std::string     module,
                  std::string     submodule,
                  rxSize          maxSize,
                  rxSize          maxPriority = DEFAULT_MAX_PRIORITY,
                  GetPriority<T>* getPriority = new GetPriorityRefCounted<T>())
        : mBuffer(maxPriority), mMaxSize(maxSize), mMaxPriority(maxPriority),
          mEscalationFactor(maxPriority), mFront(maxPriority), mBack(maxPriority),
          mEntryCount(maxPriority), mPeak(maxPriority), mDelayPeak(maxPriority),
          mStarved(maxPriority), mNotFull(maxPriority), mProcessed(maxPriority),
          mFlushed(maxPriority), mQueueCount(maxPriority), mQueuePeak(maxPriority),
          mMaxDelay(maxPriority), mGP(getPriority)
    {
        mMetrics = MetricManager::instance();

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            std::string index = StringUtil::toString(i);

            mFront[i]      = 0;
            mBack[i]       = 0;
            mEntryCount[i] = 0;
            mPeak[i]       = 0;
            mDelayPeak[i]  = 0;
            mStarved[i]    = 0;

            mProcessed[i]  = mMetrics->add(module, submodule + ".queue." + index + ".processed");
            mFlushed[i]    = mMetrics->add(module, submodule + ".queue." + index + ".flushed");
            mQueueCount[i] = mMetrics->add(module, submodule + ".queue." + index + ".count");
            mQueuePeak[i]  = mMetrics->add(module, submodule + ".queue." + index + ".peak");
            mMaxDelay[i]   = mMetrics->add(module, submodule + ".queue." + index + ".max-delay");

            mBuffer[i] = new std::vector<T>(maxSize);
        }
    }

    virtual ~PriorityQueue()
    {
        for (rxUInt i = 0; i < mMaxPriority; i++) {
            delete mBuffer[i];

            if (mMetrics != 0) {
                mMetrics->remove(mProcessed[i]);
                mMetrics->remove(mFlushed[i]);
                mMetrics->remove(mQueueCount[i]);
                mMetrics->remove(mQueuePeak[i]);
                mMetrics->remove(mMaxDelay[i]);
            }
        }

        delete mGP;
    }

    virtual void push(T t)
    {
        Synchronized guard(mMutex);
        rxULong      start, duration;
        rxUInt       priority = mGP->getPriority(t, mMaxPriority);

        start = Time::monotonicTimeInMSecs();

        while (mEntryCount[priority] == mMaxSize) { // i.e., full
            mNotFull[priority].wait(guard);
        }

        duration = Time::monotonicTimeInMSecs() - start;

        if (duration > mDelayPeak[priority]) {
            mDelayPeak[priority] = duration;
            mMetrics->setVal(mMaxDelay[priority], duration);
        }

        (*mBuffer[priority])[mBack[priority]] = t;
        mBack[priority] = (mBack[priority] + 1) % mMaxSize;
        mEntryCount[priority]++;

        mMetrics->setVal(mQueueCount[priority], mEntryCount[priority]);

        if (mEntryCount[priority] > mPeak[priority]) {
            mPeak[priority] = mEntryCount[priority];
            mMetrics->setVal(mQueuePeak[priority], mPeak[priority]);
        }

        mNotEmpty.notifyAll();
    }

    virtual void pushFront(T t)
    {
        Synchronized guard(mMutex);
        rxULong      start, duration;
        rxUInt       priority = mGP->getPriority(t, mMaxPriority);

        start = Time::monotonicTimeInMSecs();

        while (mEntryCount[priority] == mMaxSize) { // i.e., full
            mNotFull[priority].wait(guard);
        }

        duration = Time::monotonicTimeInMSecs() - start;

        if (duration > mDelayPeak[priority]) {
            mDelayPeak[priority] = duration;
            mMetrics->setVal(mMaxDelay[priority], duration);
        }

        if (mFront[priority]-- == 0) {
            mFront[priority] = mMaxSize - 1;
        }

        (*mBuffer[priority])[mFront[priority]] = t;
        mEntryCount[priority]++;

        mMetrics->setVal(mQueueCount[priority], mEntryCount[priority]);

        if (mEntryCount[priority] > mPeak[priority]) {
            mPeak[priority] = mEntryCount[priority];
            mMetrics->setVal(mQueuePeak[priority], mPeak[priority]);
        }

        mNotEmpty.notifyAll();
    }

    virtual T pop()
    {
        Synchronized guard(mMutex);
        static T     sNull(0);
        rxInt        priority;

        while ((priority = getPriorityToDequeue()) == -1) { // i.e., empty
            mNotEmpty.wait(guard);
        }

        const T f = (*mBuffer[priority])[mFront[priority]];

        // Set the object just read to the null element.
        // This is needed for reference counting.

        (*mBuffer[priority])[mFront[priority]] = sNull;
        mFront[priority] = (mFront[priority] + 1) % mMaxSize;
        mEntryCount[priority]--;

        mMetrics->inc(mProcessed[priority]);
        mMetrics->setVal(mQueueCount[priority], mEntryCount[priority]);

        mNotFull[priority].notifyAll();

        return f;
    }

    virtual void clear()
    {
        Synchronized guard(mMutex);

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            clear(i);
        }
    }

    virtual void clear(rxUInt priority)
    {
        Synchronized guard(mMutex);
        static T     sNull(0);

        mMetrics->inc(mFlushed[priority], mEntryCount[priority]);
        mMetrics->setVal(mQueueCount[priority], 0);

        for (rxUInt i = 0; i < mMaxSize; i++) {
            (*mBuffer[priority])[i] = sNull;
        }

        mEntryCount[priority] = 0;
        mFront[priority]      = 0;
        mBack[priority]       = 0;

        mNotFull[priority].notifyAll();
    }

    rxUInt getMaxSize()
    {
        return mMaxSize;
    }

    rxUInt getEntryCount()
    {
        Synchronized guard(mMutex);
        rxUInt       entryCount = 0;

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            entryCount += mEntryCount[i];
        }

        return entryCount;
    }

    rxUInt getEntryCount(rxUInt priority)
    {
        Synchronized guard(mMutex);
        return mEntryCount[priority];
    }

    rxUInt getPeak()
    {
        Synchronized guard(mMutex);
        rxUInt       peak = 0;

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            if (mPeak[i] > peak) {
                peak = mPeak[i];
            }
        }

        return peak;
    }

    rxUInt getPeak(rxUInt priority)
    {
        Synchronized guard(mMutex);
        return mPeak[priority];
    }

    bool isFull()
    {
        Synchronized guard(mMutex);

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            if (mEntryCount[i] != mMaxSize) {
                return false;
            }
        }

        return true;
    }

    bool isFull(rxUInt priority)
    {
        Synchronized guard(mMutex);
        return mEntryCount[priority] == mMaxSize;
    }

    rxUInt waitNotEmpty()
    {
        Synchronized guard(mMutex);
        rxInt        priority;

        while ((priority = getPriorityToDequeue()) == -1) {
            mNotEmpty.wait(guard);
        }

        return mEntryCount[priority];
    }

    rxUInt waitNotEmpty(const rxULong& msecs)
    {
        AbsTime absTime(msecs);
        return waitNotEmpty(absTime);
    }

    rxUInt waitNotEmpty(AbsTime& absTime)
    {
        Synchronized guard(mMutex);
        rxInt        priority;

        while ((priority = getPriorityToDequeue()) == -1) {
            mNotEmpty.wait(guard, absTime);
        }

        return mEntryCount[priority];
    }

private:
    rxInt getPriorityToDequeue()
    {
        rxInt priority  = -1;
        bool  escalated = false;

        // Search through each queue from highest priority to lowest
        // and find the priority of the first non-empty queue found.
        // This will be the candidate for the priority to dequeue but
        // the next stage will consider starvation.

        for (rxUInt i = 0; i < mMaxPriority; i++) {
            if (mEntryCount[i] > 0) {
                priority = i;
                break;
            }
        }

        if (priority == -1) {
            return -1; // ALL EMPTY
        }

        // Iterate over the non-empty queues that are of a lower priority
        // than the candidate priority and increment the starvation counters.
        // Adjust the priority to dequeue to the highest priority queue that
        // is currently starving (if one exists).

        for (rxUInt i = priority + 1; i < mMaxPriority; i++) {
            if (mEntryCount[i] > 0) {
                mStarved[i]++;

                if (!escalated && mStarved[i] >= (i * mEscalationFactor)) {
                    priority    = i;
                    mStarved[i] = 0;
                    escalated   = true;
                }
            }
        }

        return priority;
    }

    std::vector<std::vector<T>*> mBuffer;

    rxUInt               mMaxSize;
    rxUInt               mMaxPriority;
    rxUInt               mEscalationFactor;
    std::vector<rxUInt>  mFront;
    std::vector<rxUInt>  mBack;
    std::vector<rxUInt>  mEntryCount;
    std::vector<rxUInt>  mPeak;
    std::vector<rxUInt>  mDelayPeak;
    std::vector<rxUInt>  mStarved;

    mutable std::vector<ConditionVar> mNotFull;
    mutable ConditionVar              mNotEmpty;
    mutable Mutex                     mMutex;

    // Metrics related members
    MetricManager*      mMetrics;
    std::vector<rxUInt> mProcessed;
    std::vector<rxUInt> mFlushed;
    std::vector<rxUInt> mQueueCount;
    std::vector<rxUInt> mQueuePeak;
    std::vector<rxUInt> mMaxDelay;

    GetPriority<T>* mGP;
};

#endif //__PRIORITY_QUEUE_H__

