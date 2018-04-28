#ifndef __SUBSCRIPTION_H__
#define __SUBSCRIPTION_H__

#include "PubSubCmd.h"
#include "Event.h"
#include "Pushable.h"
#include "TimerCb.h"

class Publication;
class SubTimerTaskRep;
class Unsubscribe;

class Subscription
    : public PubSubCmd
{
public:
    Subscription();
    Subscription(Subscription* sub);
    Subscription(const std::string& topic, 
                 const std::string& subtopic, 
                 const std::string& context, 
                 const std::string& subcontext, 
                 Pushable<Event>*   pushable, 
                 rxInt              timeout     = 0, 
                 const std::string& mappedTopic = RX_ESREF, 
                 rxUInt             priority    = Event::DEFAULT_PRIORITY);
    virtual ~Subscription();

    virtual void process(PubSubMaps& maps);
    virtual void publish(Publication* pub);
    virtual void unsubscribe(Unsubscribe* unsub);
    virtual char getOperator();
    virtual bool isMatched();
    virtual void reset();

    std::string      getTopic();
    std::string      getSubtopic();
    std::string      getContext();
    std::string      getSubcontext();
    std::string      getMappedTopic();
    Pushable<Event>* getPushable();
    rxInt            getTimeout();

    bool isPersistent();
    bool hasTimeout();

    void scheduleTimeout();
    void scheduleTimeout(Subscription* sub);
    void removeTimeout();

    bool contains(Subscription* sub);
    bool equals(Subscription* sub);
    bool equalsSubscriber(Subscription* sub);

    void add(Subscription* sub);
    virtual Subscription* remove();

    Subscription* next();
    Subscription* prev();

    virtual std::string toString();

protected:
    virtual SubTimerTaskRep* createTimeout(const timeval* expiryTime);

    std::string      mTopic;
    std::string      mSubtopic;
    std::string      mContext;
    std::string      mSubcontext;
    Pushable<Event>* mPushable;
    std::string      mMappedTopic;
    rxInt            mTimeout;
    TimerCb          mTimerTask;
    bool             mIsMatched;
    Subscription*    mPrev;
    Subscription*    mNext;

private:
};

inline std::string Subscription::getTopic()
{
    return mTopic;
}

inline std::string Subscription::getSubtopic()
{
    return mSubtopic;
}

inline std::string Subscription::getContext()
{
    return mContext;
}

inline std::string Subscription::getSubcontext()
{
    return mSubcontext;
}

inline std::string Subscription::getMappedTopic()
{
    return mMappedTopic;
}

inline Pushable<Event>* Subscription::getPushable()
{
    return mPushable;
}

inline rxInt Subscription::getTimeout()
{
    return mTimeout;
}

inline bool Subscription::isPersistent()
{
    return mTimeout == -1;
}

inline char Subscription::getOperator()
{
    return ' ';
}

inline bool Subscription::isMatched()
{
    return mIsMatched;
}

inline void Subscription::add(Subscription* toAdd)
{
    toAdd->mNext = this;
    toAdd->mPrev = mPrev;
    mPrev->mNext = toAdd;
    mPrev        = toAdd;
}

inline Subscription* Subscription::remove()
{
    if (mPrev == 0) {
        return 0; // nop: already removed
    }

    Subscription* prev = mPrev;

    mNext->mPrev = mPrev;
    mPrev->mNext = mNext;
    mPrev        = 0;

    return prev;
}

inline Subscription* Subscription::next()
{
    return mNext;
}

inline Subscription* Subscription::prev()
{
    return mPrev;
}

inline void Subscription::reset()
{
    mIsMatched = false;
}

#endif //__SUBSCRIPTION_H__
