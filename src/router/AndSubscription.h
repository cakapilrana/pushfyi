#ifndef __AND_SUBSCRIPTION_H__
#define __AND_SUBSCRIPTION_H__

#include "AndOrSubscription.h"
#include "Event.h"

class AndSubscription
    : public AndOrSubscription
{
public:
    AndSubscription(AndSubscription* sub);
    AndSubscription(const std::string& topic, AndOrSubscription* sub, AndOrSubscription*& root);
    AndSubscription(std::vector<std::string>* topic, 
                    const std::string&        subtopic, 
                    const std::string&        context, 
                    const std::string&        subcontext, 
                    Pushable<Event>*          pushable, 
                    rxInt                     timeout     = 0, 
                    const std::string&        mappedTopic = RX_ESREF, 
                    rxUInt                    priority    = Event::DEFAULT_PRIORITY);
    virtual ~AndSubscription();

    virtual void publish(Publication* pub);
    virtual void unsubscribe(Unsubscribe* unsub);
    virtual bool isMatched();
    virtual void reset();

    virtual AndOrSubscription* createChild(const std::string& topic, AndOrSubscription*& root);
    virtual char getOperator();

private:
    void setEvent(Event& event);
    bool allMatched();

    Event mEvent;
    bool  mAllMatched;
};

inline bool AndSubscription::isMatched()
{
    return mAllMatched;
}

inline char AndSubscription::getOperator()
{
    return '&';
}

inline AndOrSubscription* AndSubscription::createChild(const std::string& topic, AndOrSubscription*& root)
{
    return new AndSubscription(topic, this, root);
}

inline void AndSubscription::setEvent(Event& event)
{
    AndSubscription* sub = this;

    do {
        sub->mEvent = event;
        sub = (AndSubscription*) sub->mNextSibling;
    } while (sub != this);
}

#endif //__AND_SUBSCRIPTION_H__
