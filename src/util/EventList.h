/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: EventList Class For Nested System Events.
Uses tinyxml

*********************************************************
*/
#ifndef __EVENT_LIST_H__
#define __EVENT_LIST_H__

#include <vector>
#include "Event.h"

class EventList;

class EventListRep
    : public EventRep
{
public:
    EventListRep();
    EventListRep(const std::string& topic);
    EventListRep(const std::string& topic, const std::string& subtopic);
    EventListRep(const std::string& topic, const std::string& subtopic, const std::string& device);
    EventListRep(const EventRep& event);
    EventListRep(const EventListRep& event);
    EventListRep(const Event& event);
    virtual ~EventListRep();

    rxSize size();
    void resize(rxSize size);
    bool isEmpty();

    Event getEvent(rxSize index);
    Event removeFirst();
    rxUInt remove(rxUInt index, rxUInt length);

    void addEvent(const Event& event);
    void addEvents(EventList& events);
    void erase();

    virtual bool isCompound();

    virtual EventRep* clone();
    virtual eRefType getType();
    virtual std::string toXML();
    virtual std::string toXMLHeader();

private:
    std::vector<Event> mList;
};

inline rxSize EventListRep::size()
{
    return mList.size();
}

inline void EventListRep::resize(rxSize size)
{
    return mList.resize(size, 0);
}

inline bool EventListRep::isEmpty()
{
    return mList.size() == 0;
}

inline Event EventListRep::getEvent(rxSize index)
{
    if (index >= mList.size()) {
        return 0;
    }

    return mList.at(index);
}

inline Event EventListRep::removeFirst()
{
    if (isEmpty()) {
        return 0;
    }

    Event event = mList.at(0);

    mList.erase(mList.begin());

    return event;
}

inline void EventListRep::addEvent(const Event& event)
{
    mList.push_back(event);
}

inline void EventListRep::erase()
{
    mList.clear();
}

inline bool EventListRep::isCompound()
{
    return true;
}

inline EventRep* EventListRep::clone()
{
    return new EventListRep(*this);
}

inline eRefType EventListRep::getType()
{
    return eTEventList;
}

class EventList
    : public Event
{
public:
    EventList();
    EventList(EventListRep* rep);
    EventList(const EventList& toCopy);
    virtual ~EventList();

    EventList& operator=(const EventList& toCopy);
    bool operator==(const EventList& toCompare);

    EventListRep* operator->() const;
    EventListRep* operator*() const;

    static EventListRep* toList(Event& event);

private:
};

inline EventList& EventList::operator=(const EventList& toCopy)
{
    return (EventList&) Event::operator=(toCopy);
}

inline bool EventList::operator==(const EventList& toCompare)
{
    return Event::operator==(toCompare);
}

inline EventListRep* EventList::operator->() const
{
    return (EventListRep*) mRep;
}

inline EventListRep* EventList::operator*() const
{
    return (EventListRep*) mRep;
}

#endif //__EVENT_LIST_H__
