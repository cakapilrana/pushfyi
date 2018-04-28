/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: LONG Type Wrapper for casting support

*********************************************************
*/
#ifndef __LONG_H__
#define __LONG_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class LongRep
    : public VarRep
{
public:
    LongRep();
    LongRep(rxLong value);
    LongRep(const LongRep& object);
    virtual ~LongRep();

    rxLong get();
    void set(rxLong value);

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
    rxLong mValue;
};

inline rxLong LongRep::get()
{
    return mValue;
}

inline void LongRep::set(rxLong value)
{
    mValue = value;
}

inline eRefType LongRep::getType()
{
    return eTLong;
}

class Long
    : public Var
{
public:
    Long();
    Long(rxLong value);
    Long(LongRep* rep);
    Long(const Long& toCopy);
    virtual ~Long();

    Long& operator=(const Long& toCopy);
    bool operator==(const Long& toCompare);

    LongRep* operator->() const;
    LongRep* operator*() const;

private:
};

inline Long& Long::operator=(const Long& toCopy)
{
    return (Long&) Ref::operator=(toCopy);
}

inline bool Long::operator==(const Long& toCompare)
{
    return mRep == toCompare.mRep;
}

inline LongRep* Long::operator->() const
{
    return (LongRep*) mRep;
}

inline LongRep* Long::operator*() const
{
    return (LongRep*) mRep;
}

#endif //__LONG_H__
