/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: String Type Wrapper for casting support

*********************************************************
*/
#ifndef __STRING_H__
#define __STRING_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class StringRep
    : public VarRep
{
public:
    StringRep();
    StringRep(const std::string& value);
    StringRep(const StringRep& object);
    virtual ~StringRep();

    std::string get();
    void set(const std::string& value);

    virtual bool toBool();
    virtual rxInt toInt();
    virtual rxUInt toUInt();
    virtual rxLong toLong();
    virtual rxULong toULong();
    virtual rxFloat toFloat();
    virtual rxDouble toDouble();
    virtual std::string toString();
    virtual Ref::Rep* toRep();

    virtual void setBool(bool value);
    virtual void setInt(rxInt value);
    virtual void setUInt(rxUInt value);
    virtual void setLong(rxLong value);
    virtual void setULong(rxULong value);
    virtual void setFloat(rxFloat value);
    virtual void setDouble(rxDouble value);
    virtual void setString(const std::string& value);
    virtual void setRep(Ref::Rep* value);

    virtual eRefType getType();

private:
    std::string mValue;
};

inline std::string StringRep::get()
{
    return mValue;
}

inline void StringRep::set(const std::string& value)
{
    mValue = value;
}

inline eRefType StringRep::getType()
{
    return eTString;
}

class String
    : public Var
{
public:
    String();
    String(const std::string& value);
    String(StringRep* rep);
    String(const String& toCopy);
    virtual ~String();

    String& operator=(const String& toCopy);
    bool operator==(const String& toCompare);

    StringRep* operator->() const;
    StringRep* operator*() const;

private:
};

inline String& String::operator=(const String& toCopy)
{
    return (String&) Ref::operator=(toCopy);
}

inline bool String::operator==(const String& toCompare)
{
    return mRep == toCompare.mRep;
}

inline StringRep* String::operator->() const
{
    return (StringRep*) mRep;
}

inline StringRep* String::operator*() const
{
    return (StringRep*) mRep;
}

#endif //__STRING_H__
