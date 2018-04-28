/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: UNSIGNED INT Type Wrapper for casting support

*********************************************************
*/
#ifndef __UINT_H__
#define __UINT_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class UIntRep
    : public VarRep
{
public:
    UIntRep();
    UIntRep(rxUInt value);
    UIntRep(const UIntRep& object);
    virtual ~UIntRep();

    rxUInt get();
    void set(rxUInt value);

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
    rxUInt mValue;
};

inline rxUInt UIntRep::get()
{
    return mValue;
}

inline void UIntRep::set(rxUInt value)
{
    mValue = value;
}

inline eRefType UIntRep::getType()
{
    return eTUInt;
}

class UInt
    : public Var
{
public:
    UInt();
    UInt(rxUInt value);
    UInt(UIntRep* rep);
    UInt(const UInt& toCopy);
    virtual ~UInt();

    UInt& operator=(const UInt& toCopy);
    bool operator==(const UInt& toCompare);

    UIntRep* operator->() const;
    UIntRep* operator*() const;

private:
};

inline UInt& UInt::operator=(const UInt& toCopy)
{
    return (UInt&) Ref::operator=(toCopy);
}

inline bool UInt::operator==(const UInt& toCompare)
{
    return mRep == toCompare.mRep;
}

inline UIntRep* UInt::operator->() const
{
    return (UIntRep*) mRep;
}

inline UIntRep* UInt::operator*() const
{
    return (UIntRep*) mRep;
}

#endif //__UINT_H__
