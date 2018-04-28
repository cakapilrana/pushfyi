#ifndef __PUBSUB_FUTURE_H__
#define __PUBSUB_FUTURE_H__

#include <string>
#include "Event.h"
#include "pushfyi-types.h"
#include "ConditionVar.h"
#include "Mutex.h"
#include "Pushable.h"
#include "Synchronized.h"

class PubSubFuture
    : public Pushable<Event>
{
public:
    PubSubFuture(const std::string& topic,
                 const std::string& subtopic,
                 rxUInt             timeout,
                 rxUInt             priority = Event::DEFAULT_PRIORITY);
    virtual ~PubSubFuture();

    void push(Event event);
    void pushFront(Event event);

    Event getEvent();

private:
    static std::string createUniqueId();

    static rxUInt UNIQUE_ID;
    static Mutex  ID_MUTEX;

    Event        mEvent;
    ConditionVar mNotPublished;
    Mutex        mMutex;
};

inline std::string PubSubFuture::createUniqueId()
{
    Synchronized lock(ID_MUTEX);
    return StringUtil::toString(UNIQUE_ID++);
}

#endif //__PUBSUB_FUTURE_H__
