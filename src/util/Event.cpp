/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Event Class For All System Events.
Uses tinyxml

*********************************************************
*/
#include "Event.h"
#include "EventList.h"
#include "tinyxml.h"

using namespace std;

static const timeval TZERO = {0, 0};

const std::string& EventRep::TRANSCTX = "transContext";

EventRep::EventRep()
    : mPriority(Event::DEFAULT_PRIORITY)
{
    // nop
}

EventRep::EventRep(const string& topic)
    : mTopic(topic), mSubtopic(), mDevice(), mContext(),
      mSubcontext(), mServiceId(), mPriority(Event::DEFAULT_PRIORITY), 
      mParams(), mReadTime(TZERO)
{
    // nop
}

EventRep::EventRep(const string& topic, const string& subtopic)
    : mTopic(topic), mSubtopic(subtopic), mDevice(), mContext(),
      mSubcontext(), mServiceId(), mPriority(Event::DEFAULT_PRIORITY), 
      mParams(), mReadTime(TZERO)
{
    // nop
}

EventRep::EventRep(const string& topic, const string& subtopic, const string& device)
    : mTopic(topic), mSubtopic(subtopic), mDevice(device), mContext(),
      mSubcontext(), mServiceId(), mPriority(Event::DEFAULT_PRIORITY), 
      mParams(), mReadTime(TZERO)
{
    // nop
}

EventRep::EventRep(const EventRep& toCopy)
    : Ref::Rep(), mTopic(toCopy.mTopic), mSubtopic(toCopy.mSubtopic), 
      mDevice(toCopy.mDevice), mContext(toCopy.mContext), 
      mSubcontext(toCopy.mSubcontext), mServiceId(toCopy.mServiceId), 
      mPriority(toCopy.mPriority), mParams(toCopy.mParams), mReadTime(TZERO)
{
    // nop
}

EventRep::~EventRep()
{
    // nop
}

inline string elementXML(const string& name, string& value)
{   
    string result("");
    if(value.size()>0) {
        result = "<"+name+">" + StringUtil::encodeXmlChars(value) + "</" + name + ">\n";
    }
    return result;
}

string EventRep::bodyToXML()
{
    string        paramBuf;
    HashtableIter iter = mParams.begin();
    HashtableIter end  = mParams.end();
    string        pri  = StringUtil::toString(mPriority);

    if (iter != end) {
        paramBuf = "<params>\n";

        for ( ; iter != end; iter++) {
            Var&   var   = iter->second;
            string key   = iter->first;
            string value = !var.isNull() ? var->toString() : "";

            if (value.size() > 0) {
                if (value[0] != '<') {
                    StringUtil::encodeXmlChars(value);
                }

                paramBuf += "<var name=\"" + key + "\">" + value + "</var>\n";
            }
/*  I drop support to transfer empty value, because it broke some web page right now            
            else {
                paramBuf += "<var name=\"" + key + "\"/>\n";
            }
*/
        }

        paramBuf += "</params>\n";
    }

    return  elementXML("topic" , mTopic) +
            elementXML("subtopic" , mSubtopic) +
            elementXML("device" , mDevice) +
            elementXML("context" , mContext) +
            elementXML("subcontext" , mSubcontext) +
            elementXML("service" , mServiceId) +
            elementXML("priority" , pri) +
            paramBuf;
}

string EventRep::toXML()
{
    string result("<ims-event>\n");
    result += bodyToXML();
    result += "</ims-event>\n";

    return result;
}

bool EventRep::toEvent(TiXmlNode* node, Event& event)
{
    if (node == 0 || node->Type() != TiXmlNode::ELEMENT) {
        return false;
    }

    const char* value = node->Value();

    if (::strcmp(value, "ims-event") == 0) {
        event = new EventRep();
    }
    else if (::strcmp(value, "ims-event-list") == 0) {
        EventListRep* list     = new EventListRep();
        TiXmlNode*    size     = node->IterateChildren("size", 0);
        TiXmlNode*    previous = 0;

        event = list;

        // Check <rx-event-list><size>N</size></rx-event-list>
        // variation of XML serialization of an EventList.
        if (size != 0) {
            TiXmlNode* body = size->FirstChild();

            if (body != 0) {
                list->resize(::strtoul(body->Value(), 0, 10));
            }
        }
        else {
            TiXmlNode* current  = node->IterateChildren("ims-event", 0);
            while (current != 0) {
                Event nested(0);

                if (toEvent(current, nested) && !nested.isNull()) {
                    list->addEvent(nested);
                }

                previous = current;
                current  = node->IterateChildren("ims-event", previous);
            }

            current  = node->IterateChildren("ims-event-list", 0);
            while (current != 0) {
                Event nested(0);

                if (toEvent(current, nested) && !nested.isNull()) {
                    list->addEvent(nested);
                }

                previous = current;
                current  = node->IterateChildren("ims-event-list", previous);
            }
        }
    }
    else {
        return false;
    }

    TiXmlNode* topic = node->IterateChildren("topic", 0);

    if (topic != 0) {
        TiXmlNode* body = topic->FirstChild();

        if (body != 0) {
            event->mTopic = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* subtopic = node->IterateChildren("subtopic", 0);

    if (subtopic != 0) {
        TiXmlNode* body = subtopic->FirstChild();

        if (body != 0) {
            event->mSubtopic = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* device = node->IterateChildren("device", 0);

    if (device != 0) {
        TiXmlNode* body = device->FirstChild();

        if (body != 0) {
            event->mDevice = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* context = node->IterateChildren("context", 0);

    if (context != 0) {
        TiXmlNode* body = context->FirstChild();

        if (body != 0) {
            event->mContext = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* subcontext = node->IterateChildren("subcontext", 0);

    if (subcontext != 0) {
        TiXmlNode* body = subcontext->FirstChild();

        if (body != 0) {
            event->mSubcontext = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* service = node->IterateChildren("service", 0);

    if (service != 0) {
        TiXmlNode* body = service->FirstChild();

        if (body != 0) {
            event->mServiceId = StringUtil::decodeXmlChars(body->Value());
        }
    }

    TiXmlNode* priority = node->IterateChildren("priority", 0);

    if (priority != 0) {
        TiXmlNode* body = priority->FirstChild();

        if (body != 0) {
            event->mPriority = ::strtoul(StringUtil::decodeXmlChars(body->Value()).c_str(), 0, 10);
        }
    }

    TiXmlNode* params = node->IterateChildren("params", 0);

    if (params != 0) {
        TiXmlNode* child = params->FirstChild();

        for (; child != 0; child = child->NextSibling()) {
            TiXmlElement* element = child->ToElement();
            if(element == 0){
                continue;
            }
            //we pass <var name="key">value</var> element
            //or the short way <var name="key" value="avalue" />
        #ifdef TIXML_USE_STL
            if(element->ValueStr().compare("var") !=0 ){
        #else
            if(::strcmp(element->Value(), "var") !=0 ){
        #endif
                continue;
            }
            const char * name = element->Attribute("name");
            if(name == 0){
                continue;
            }
            TiXmlNode*   body = element->FirstChild();
            const char* value = element->Attribute("value");
            if (body == 0) {
            	if(value){
            		event->set(string(name), value);
            	}
            	else{
	            	event->set(string(name), "");
	            }
            }
            else {
                if(body->Type() == TiXmlNode::TEXT){
                	TiXmlText *  text = body->ToText();
                   	value = body->Value();
                	if(text->CDATA()){
	                    event->set(string(name), string("<![CDATA[") + value + "]]>");
                	}
                	else{
	                    event->set(string(name), StringUtil::decodeXmlChars(value));
                    }
                }
                else{
                    TiXmlPrinter printer;
                    printer.SetStreamPrinting();

                    body->Accept( &printer );
                    event->set(string(name), printer.CStr());
                                    
                }
            }
        }
    }

    return true;
}

const rxUInt Event::DEFAULT_PRIORITY = 2;

Event Event::toEvent(TiXmlNode* node)
{
    Event event(0);
    EventRep::toEvent(node, event);
    return event;
}

Event Event::toEvent(const string& xml)
{
    Event event(0);
    TiXmlDocument doc;
    try{
        doc.Parse(xml.c_str());
        event = toEvent(doc.RootElement());
    }
    catch(...){
        
    }
    return event;
}

Event::Event()
    : TRef<EventRep>()
{
    // nop
}

Event::Event(EventRep* rep)
    : TRef<EventRep>(rep)
{
    // nop
}

Event::Event(const Event& toCopy)
    : TRef<EventRep>(toCopy)
{
    // nop
}

Event::~Event()
{
    // nop
}

EventIterRep::EventIterRep(Event& event)
    : mEvent(event), mIndex(0)
{
    // nop
}

EventIterRep::~EventIterRep()
{
    // nop
}

Event EventIterRep::next()
{
    if (mEvent.isNull()) {
        return 0;
    }

    if (!mEvent->isCompound()) {
        return (mIndex++ == 0) ? mEvent : 0;
    }

    EventListRep* list = (EventListRep*) *mEvent;

    return list->getEvent(mIndex++);
}
