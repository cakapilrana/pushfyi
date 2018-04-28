#ifndef __ANDOR_UNSUBSCRIBE_H__
#define __ANDOR_UNSUBSCRIBE_H__

#include "Unsubscribe.h"

class AndSubscription;
class Subscription;

class AndOrUnsubscribe
    : public Unsubscribe
{
public:
    AndOrUnsubscribe(std::vector<std::string>* topics, 
                     const std::string&        subtopic, 
                     const std::string&        context, 
                     const std::string&        subcontext, 
                     Pushable<Event>*          pushable, 
                     bool                      isTimeout = false, 
                     rxUInt                    priority  = Event::DEFAULT_PRIORITY);
    virtual ~AndOrUnsubscribe();

    virtual void process(PubSubMaps& maps);

    std::vector<std::string>* getTopics();
    std::string getTopicExpression();

protected:
    std::vector<std::string> mTopics;
};

inline std::vector<std::string>* AndOrUnsubscribe::getTopics()
{
    return &mTopics;
}

inline std::string AndOrUnsubscribe::getTopicExpression()
{
    rxUInt      length = mTopics.size();
    std::string topics;

    for (rxUInt i = 0; i < length; i++) {
        topics += mTopics[i];

        if (i + 1 < length) {
            topics += "|";
        }
    }

    return topics;
}


#endif //__ANDOR_UNSUBSCRIBE_H__
