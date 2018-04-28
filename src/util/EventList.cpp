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
#include "EventList.h"

using namespace std;

EventListRep::EventListRep()
{
    // nop
}

EventListRep::EventListRep(const string& topic)
    : EventRep(topic)
{
    // nop
}

EventListRep::EventListRep(const string& topic, const string& subtopic)
    : EventRep(topic, subtopic)
{
    // nop
}

EventListRep::EventListRep(const string& topic, const string& subtopic, const string& device)
    : EventRep(topic, subtopic, device)
{
    // nop
}

EventListRep::EventListRep(const EventRep& toCopy)
    : EventRep(toCopy)
{
    if (((EventRep&)toCopy).isCompound()) {
        mList = ((EventListRep&) toCopy).mList;
    }
}

EventListRep::EventListRep(const EventListRep& toCopy)
    : EventRep(toCopy), mList(toCopy.mList)
{
    // nop
}

EventListRep::EventListRep(const Event& toCopy)
    : EventRep(**toCopy)
{
    addEvent(toCopy);
}

EventListRep::~EventListRep()
{
    // nop
}

void EventListRep::addEvents(EventList& events)
{
    if (events.isNull()) {
        return;
    }

    for (rxUInt i = 0; i < events->size(); i++) {
        Event event = events->getEvent(i);
        addEvent(event);
    }
}

rxUInt EventListRep::remove(rxUInt index, rxUInt length)
{
    if (index >= mList.size()) {
        return 0;
    }

    rxUInt size      = mList.size();
    rxUInt maxLength = size - index;

    if (length > maxLength) {
        length = maxLength;
    }

    vector<Event>::iterator start = mList.begin() + index;
    vector<Event>::iterator end   = start + length;

    mList.erase(start, end);

    return size - mList.size();
}

string EventListRep::toXML()
{

    string result("<ims-event-list>\n");
    result += bodyToXML();

    for (rxUInt i = 0; i < size(); i++) {
        Event event = getEvent(i);

        if (!event.isNull()) {
            result += event->toXML();
        }
        else {
            result += "  <ims-event></ims-event>\n";
        }
    }

    result += "</ims-event-list>\n";

    return result;
}

string EventListRep::toXMLHeader()
{

    string result("<ims-event-list>\n");
    result += "  <size>" + StringUtil::toString(mList.size()) + "</size>\n";
    result += bodyToXML();
    result += "</ims-event-list>\n";

    return result;
}

EventList::EventList()
    : Event(new EventListRep())
{
    // nop
}

EventList::EventList(EventListRep* rep)
    : Event(rep)
{
    // nop
}

EventList::EventList(const EventList& toCopy)
    : Event(toCopy)
{
    // nop
}

EventList::~EventList()
{
    // nop
}

EventListRep* EventList::toList(Event& event)
{
    if (event.isNull()) {
        return 0;
    }

    if (event->isCompound()) {
        return (EventListRep*) *event;
    }

    return new EventListRep(event);
}
