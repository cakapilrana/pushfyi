#ifndef __UNSUBSCRIBE_H__
#define __UNSUBSCRIBE_H__

#include <string>
#include "PubSubCmd.h"
#include "Event.h"
#include "Pushable.h"

class Subscription;

class Unsubscribe
    : public PubSubCmd
{
public:
    Unsubscribe(const std::string& topic, 
                const std::string& subtopic, 
                const std::string& context, 
                const std::string& subcontext, 
                Pushable<Event>*   pushable, 
                bool               isTimeout = false, 
                rxUInt             priority  = Event::DEFAULT_PRIORITY);
    virtual ~Unsubscribe();

    virtual void process(PubSubMaps& maps);

    bool equals(Subscription* sub);

    std::string      getTopic();
    std::string      getSubtopic();
    std::string      getContext();
    std::string      getSubcontext();
    Pushable<Event>* getPushable();
    bool             isTimeout();
    bool             isProcessed();
    void             processed();

protected:
    std::string      mTopic;
    std::string      mSubtopic;
    std::string      mContext;
    std::string      mSubcontext;
    Pushable<Event>* mPushable;
    bool             mIsTimeout;
    bool             mIsProcessed;
};

inline std::string Unsubscribe::getTopic()
{
    return mTopic;
}

inline std::string Unsubscribe::getSubtopic()
{
    return mSubtopic;
}

inline std::string Unsubscribe::getContext()
{
    return mContext;
}

inline std::string Unsubscribe::getSubcontext()
{
    return mSubcontext;
}

inline Pushable<Event>* Unsubscribe::getPushable()
{
    return mPushable;
}

inline bool Unsubscribe::isTimeout()
{
    return mIsTimeout;
}

inline bool Unsubscribe::isProcessed()
{
    return mIsProcessed;
}

inline void Unsubscribe::processed()
{
    mIsProcessed = true;
}

#endif //__UNSUBSCRIBE_H__
