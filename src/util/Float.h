/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: FLOAT Type Wrapper for casting support

*********************************************************
*/
#ifndef __FLOAT_H__
#define __FLOAT_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class FloatRep
    : public VarRep
{
public:
    FloatRep();
    FloatRep(rxFloat value);
    FloatRep(const FloatRep& object);
    virtual ~FloatRep();

    rxFloat get();
    void set(rxFloat value);

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
    rxFloat mValue;
};

inline rxFloat FloatRep::get()
{
    return mValue;
}

inline void FloatRep::set(rxFloat value)
{
    mValue = value;
}

inline eRefType FloatRep::getType()
{
    return eTFloat;
}

class Float
    : public Var
{
public:
    Float();
    Float(rxFloat value);
    Float(FloatRep* rep);
    Float(const Float& toCopy);
    virtual ~Float();

    Float& operator=(const Float& toCopy);
    bool operator==(const Float& toCompare);

    FloatRep* operator->() const;
    FloatRep* operator*() const;

private:
};

inline Float& Float::operator=(const Float& toCopy)
{
    return (Float&) Ref::operator=(toCopy);
}

inline bool Float::operator==(const Float& toCompare)
{
    return mRep == toCompare.mRep;
}

inline FloatRep* Float::operator->() const
{
    return (FloatRep*) mRep;
}

inline FloatRep* Float::operator*() const
{
    return (FloatRep*) mRep;
}

#endif //__DOUBLE_H__
