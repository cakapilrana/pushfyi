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
#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/time.h>
#include <string>
#include "pushfyi-types.h"
#include "Params.h"
#include "TRef.h"
#include "Object.h"

class TiXmlNode;

class Event;

/**
 * @ingroup base
 */
class EventRep
    : public Ref::Rep
{
public:
    EventRep();
    EventRep(const std::string& topic);
    EventRep(const std::string& topic, const std::string& subtopic);
    EventRep(const std::string& topic, const std::string& subtopic, const std::string& device);
    EventRep(const EventRep& event);
    virtual ~EventRep();

    static const std::string& TRANSCTX;
    static bool toEvent(TiXmlNode* node, Event& event);

    std::string getContext();
    void setContext(const std::string& context);

    std::string getSubcontext();
    void setSubcontext(const std::string& subcontext);

    std::string getTransContext();
    void setTransContext(const std::string& transContext);

    std::string getServiceId();
    void setServiceId(const std::string& serviceId);

    std::string getTopic();
    void setTopic(const std::string& topic);

    std::string getSubtopic();
    void setSubtopic(const std::string& subtopic);

    std::string getDevice();
    void setDevice(const std::string& device);

    rxUInt getPriority();
    void setPriority(rxUInt priority);

    std::string get(const std::string& key, const std::string& def = RX_ESREF) const;
    void set(const std::string& key, const std::string& value);

    std::string getString(const std::string& key, const std::string& def = RX_ESREF);
    void setString(const std::string& key, const std::string& value);

    rxUInt getUInt(const std::string& key, rxUInt def = 0) const;
    void setUInt(const std::string& key, rxUInt value);

    rxInt getInt(const std::string& key, rxInt def = 0);
    void setInt(const std::string& key, rxInt value);

    rxULong getULong(const std::string& key, rxULong def = 0LL);
    void setULong(const std::string& key, rxULong value);

    rxLong getLong(const std::string& key, rxLong def = 0LL);
    void setLong(const std::string& key, rxLong value);

    rxFloat getFloat(const std::string& key, rxFloat def = 0.0);
    void setFloat(const std::string& key, rxFloat value);

    rxDouble getDouble(const std::string& key, rxDouble def = 0.0);
    void setDouble(const std::string& key, rxDouble value);

    bool getBool(const std::string& key, bool def = 0);
    void setBool(const std::string& key, bool value);

    void* getObject(const std::string& key, void* def = 0);
    void setObject(const std::string& key, void* value);

    Ref::Rep* getRef(const std::string& key, Ref::Rep* def = 0);
    void setRef(const std::string& key, Ref::Rep* value);

    Var& getVar(const std::string& key);
    Var& getVar(const std::string& key, Var& def);
    void setVar(const std::string& key, Var& value);

    void remove(const std::string& key);
    bool contains(const std::string& key);

    void clear();

    Params& getParams();

    virtual bool isCompound();

    virtual EventRep* clone();
    virtual eRefType getType();
    virtual std::string toString();
    virtual std::string toXML();

    timeval getReadTime(timeval* t = 0) const;
    void setReadTime(const timeval* t = 0);

protected:
    virtual std::string bodyToXML();

    std::string mTopic;
    std::string mSubtopic;
    std::string mDevice;
    std::string mContext;
    std::string mSubcontext;
    std::string mServiceId;
    rxUInt      mPriority;
    Params      mParams;
    timeval mReadTime;
};

inline std::string EventRep::getContext()
{
    return mContext;
}

inline void EventRep::setContext(const std::string& context)
{
    mContext = context;
}

inline std::string EventRep::getSubcontext()
{
    return mSubcontext;
}

inline void EventRep::setSubcontext(const std::string& subcontext)
{
    mSubcontext = subcontext;
}

inline std::string EventRep::getTransContext()
{
    return get(TRANSCTX);
}

inline void EventRep::setTransContext(const std::string& transContext)
{
    set(TRANSCTX, transContext);
}

inline std::string EventRep::getServiceId()
{
    return mServiceId;
}

inline void EventRep::setServiceId(const std::string& serviceId)
{
    mServiceId = serviceId;
}

inline std::string EventRep::getTopic()
{
    return mTopic;
}

inline void EventRep::setTopic(const std::string& topic)
{
    mTopic = topic;
}

inline std::string EventRep::getSubtopic()
{
    return mSubtopic;
}

inline void EventRep::setSubtopic(const std::string& subtopic)
{
    mSubtopic = subtopic;
}

inline std::string EventRep::getDevice()
{
    return mDevice;
}

inline void EventRep::setDevice(const std::string& device)
{
    mDevice = device;
    set("deviceId", device);
}

inline rxUInt EventRep::getPriority()
{
    return mPriority;
}

inline void EventRep::set(const std::string& key, const std::string& value)
{
    mParams.set(key, value);
}

inline std::string EventRep::getString(const std::string& key, const std::string& def)
{
    return get(key, def);
}

inline void EventRep::setString(const std::string& key, const std::string& value)
{
    set(key, value);
}

inline rxUInt EventRep::getUInt(const std::string& key, rxUInt def) const
{
    return mParams.getUInt(key, def);
}

inline void EventRep::setUInt(const std::string& key, rxUInt value)
{
    mParams.setUInt(key, value);
}

inline rxInt EventRep::getInt(const std::string& key, rxInt def)
{
    return mParams.getInt(key, def);
}

inline void EventRep::setInt(const std::string& key, rxInt value)
{
    mParams.setInt(key, value);
}

inline rxULong EventRep::getULong(const std::string& key, rxULong def)
{
    return mParams.getULong(key, def);
}

inline void EventRep::setULong(const std::string& key, rxULong value)
{
    mParams.setULong(key, value);
}

inline rxLong EventRep::getLong(const std::string& key, rxLong def)
{
    return mParams.getLong(key, def);
}

inline void EventRep::setLong(const std::string& key, rxLong value)
{
    mParams.setLong(key, value);
}

inline rxFloat EventRep::getFloat(const std::string& key, rxFloat def)
{
    return mParams.getFloat(key, def);
}

inline void EventRep::setFloat(const std::string& key, rxFloat value)
{
    mParams.setFloat(key, value);
}

inline rxDouble EventRep::getDouble(const std::string& key, rxDouble def)
{
    return mParams.getDouble(key, def);
}

inline void EventRep::setDouble(const std::string& key, rxDouble value)
{
    mParams.setDouble(key, value);
}

inline bool EventRep::getBool(const std::string& key, bool def)
{
    return mParams.getBool(key, def);
}

inline void EventRep::setBool(const std::string& key, bool value)
{
    mParams.setBool(key, value);
}

inline void* EventRep::getObject(const std::string& key, void* def)
{
    return mParams.getObject(key, def);
}

inline void EventRep::setObject(const std::string& key, void* value)
{
    mParams.setObject(key, value);
}

inline Ref::Rep* EventRep::getRef(const std::string& key, Ref::Rep* def)
{
    return mParams.getRef(key, def);
}

inline void EventRep::setRef(const std::string& key, Ref::Rep* value)
{
    mParams.setRef(key, value);
}

inline Var& EventRep::getVar(const std::string& key)
{
    return mParams.getVar(key);
}

inline Var& EventRep::getVar(const std::string& key, Var& def)
{
    return mParams.getVar(key, def);
}

inline void EventRep::setVar(const std::string& key, Var& value)
{
    mParams.setVar(key, value);
}

inline void EventRep::remove(const std::string& key)
{
    mParams.remove(key);
}

inline bool EventRep::contains(const std::string& key)
{
    return mParams.contains(key);
}

inline void EventRep::clear()
{
    mParams.clear();
}

inline Params& EventRep::getParams()
{
    return mParams;
}

inline bool EventRep::isCompound()
{
    return false;
}

inline EventRep* EventRep::clone()
{
    return new EventRep(*this);
}

inline eRefType EventRep::getType()
{
    return eTEvent;
}

inline std::string EventRep::toString()
{
    return toXML();
}

inline timeval EventRep::getReadTime(timeval* t) const
{
    if(t != NULL){
        t->tv_sec = mReadTime.tv_sec;
        t->tv_usec = mReadTime.tv_usec;
    }
    return mReadTime;
}


inline void EventRep::setReadTime(const timeval* t)
{
    if(t != NULL){
        mReadTime.tv_sec = t->tv_sec;
        mReadTime.tv_usec = t->tv_usec;
    }else{
        mReadTime.tv_sec = 0;
        mReadTime.tv_usec = 0;
    }
}

class Event
    : public TRef<EventRep>
{
public:
    Event();
    Event(EventRep* rep);
    Event(const Event& toCopy);
    virtual ~Event();

    Event& operator=(const Event& toCopy);
    bool operator==(const Event& toCompare);

    static Event toEvent(const std::string& xml);

    static const rxUInt DEFAULT_PRIORITY;
protected:
    static Event toEvent(TiXmlNode* node);
};

inline Event& Event::operator=(const Event& toCopy)
{
    return (Event&) TRef<EventRep>::operator=(toCopy);
}

inline bool Event::operator==(const Event& toCompare)
{
    return mRep == toCompare.mRep;
}

class EventIterRep
    : public Ref::Rep
{
public:
    EventIterRep(Event& event);
    virtual ~EventIterRep();

    Event next();

    virtual eRefType getType();

private:
    Event  mEvent;
    rxUInt mIndex;
};

inline eRefType EventIterRep::getType()
{
    return eTEventIter;
}

inline void EventRep::setPriority(rxUInt priority)
{
    mPriority = priority;
}

inline std::string EventRep::get(const std::string& key, const std::string& def) const
{
    return mParams.get(key, def);
}

typedef TRef<EventIterRep> EventIter;


#endif //__EVENT_H__
