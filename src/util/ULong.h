/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: UNSIGNED LONG Type Wrapper for casting support

*********************************************************
*/
#ifndef __ULONG_H__
#define __ULONG_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class ULongRep
    : public VarRep
{
public:
    ULongRep();
    ULongRep(rxULong value);
    ULongRep(const ULongRep& object);
    virtual ~ULongRep();

    rxULong get();
    void set(rxULong value);

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
    rxULong mValue;
};

inline rxULong ULongRep::get()
{
    return mValue;
}

inline void ULongRep::set(rxULong value)
{
    mValue = value;
}

inline eRefType ULongRep::getType()
{
    return eTULong;
}

class ULong
    : public Var
{
public:
    ULong();
    ULong(rxULong value);
    ULong(ULongRep* rep);
    ULong(const ULong& toCopy);
    virtual ~ULong();

    ULong& operator=(const ULong& toCopy);
    bool operator==(const ULong& toCompare);

    ULongRep* operator->() const;
    ULongRep* operator*() const;

private:
};

inline ULong& ULong::operator=(const ULong& toCopy)
{
    return (ULong&) Ref::operator=(toCopy);
}

inline bool ULong::operator==(const ULong& toCompare)
{
    return mRep == toCompare.mRep;
}

inline ULongRep* ULong::operator->() const
{
    return (ULongRep*) mRep;
}

inline ULongRep* ULong::operator*() const
{
    return (ULongRep*) mRep;
}

#endif //__ULONG_H__
