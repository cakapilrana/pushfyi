#ifndef __PUBLICATION_H__
#define __PUBLICATION_H__

#include <list>
#include <string>
#include "PubSubCmd.h"
#include "Event.h"

class Subscription;
class Publication;
class Unpublish;

typedef std::list<Publication*> Publications;
typedef Publications::iterator PubIter;

class Publication
    : public PubSubCmd
{
public:
    Publication(const Event& event, rxUInt ttl = 0);
    Publication(rxUInt id, const Event& event, rxUInt ttl = 0);
    virtual ~Publication();

    virtual void process(PubSubMaps& maps);

    void publish(Subscription* sub);

    std::string getTopic();
    std::string getSubtopic();
    std::string getDevice();

    Event getEvent();
    void setEvent(Event& event);

    rxUInt getTTL();
    bool hasTTL();

    void scheduleTimeout();

    bool contains(Subscription* sub);
    bool equals(Publication* pub);
    bool equals(Unpublish* unpub);

    void add(Publications& pubs);
    bool remove(Publications& pubs, PubIter& iter);

    std::string toString();

protected:
    std::string mTopic;
    std::string mSubtopic;
    Event       mEvent;
    rxUInt      mTTL;
    rxUInt      mRefs;
};

inline std::string Publication::getTopic()
{
    return mTopic;
}

inline std::string Publication::getSubtopic()
{
    return mSubtopic;
}

inline std::string Publication::getDevice()
{
    return mEvent->getDevice();
}

inline Event Publication::getEvent()
{
    return mEvent;
}

inline void Publication::setEvent(Event& event)
{
    mEvent = event;
}

inline rxUInt Publication::getTTL()
{
    return mTTL;
}

inline bool Publication::hasTTL()
{
    return mTTL > 0;
}

inline void Publication::add(Publications& pubs)
{
    pubs.push_back(this);
    mRefs++;
}

inline bool Publication::remove(Publications& pubs, PubIter& iter)
{
    pubs.erase(iter);

    return (--mRefs == 0);
}

#endif //__PUBLICATION_H__
