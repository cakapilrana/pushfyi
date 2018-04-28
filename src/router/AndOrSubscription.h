#ifndef __ANDOR_SUBSCRIPTION_H__
#define __ANDOR_SUBSCRIPTION_H__

#include "Subscription.h"

class AndOrSubscription
    : public Subscription
{
public:
    AndOrSubscription(AndOrSubscription* sub);
    AndOrSubscription(const std::string& topic, AndOrSubscription* sub, AndOrSubscription*& root);
    AndOrSubscription(std::vector<std::string>* topic, 
                      const std::string&        subtopic, 
                      const std::string&        context, 
                      const std::string&        subcontext, 
                      Pushable<Event>*          pushable, 
                      rxInt                     timeout     = 0, 
                      const std::string&        mappedTopic = RX_ESREF, 
                      rxUInt                    priority    = Event::DEFAULT_PRIORITY);
    virtual ~AndOrSubscription();

    virtual void process(PubSubMaps& maps);

    virtual AndOrSubscription* createChild(const std::string& topic, AndOrSubscription*& root) = 0;

    std::vector<std::string>* getTopics();
    std::string getTopicExpression();

    virtual Subscription* remove();

    AndOrSubscription*& prevSibling();
    AndOrSubscription*& nextSibling();
    void removeSibling();

    virtual std::string toString();

protected:
    virtual SubTimerTaskRep* createTimeout(const timeval* expiryTime);

    std::vector<std::string>* mTopics;
    AndOrSubscription*        mPrevSibling;
    AndOrSubscription*        mNextSibling;
};

inline std::vector<std::string>* AndOrSubscription::getTopics()
{
    return mTopics;
}

inline std::string AndOrSubscription::getTopicExpression()
{
    if (mTopics == 0) {
        return mTopic;
    }

    std::vector<std::string>& topics = *mTopics;
    char                      op     = getOperator();
    rxUInt                    length = topics.size();
    std::string               result;

    for (rxUInt i = 0; i < length; i++) {
        result += topics[i];

        if (i + 1 < length) {
            result += op;
        }
    }

    return result;
}

inline AndOrSubscription*& AndOrSubscription::nextSibling()
{
    return mNextSibling;
}

inline AndOrSubscription*& AndOrSubscription::prevSibling()
{
    return mPrevSibling;
}

inline void AndOrSubscription::removeSibling()
{
    if (mNextSibling == this) {
        return; // nop: already removed
    }

    mNextSibling->mPrevSibling = mPrevSibling;
    mPrevSibling->mNextSibling = mNextSibling;
    mNextSibling               = this;
}

#endif //__ANDOR_SUBSCRIPTION_H__
