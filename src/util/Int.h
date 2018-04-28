/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: INT Type Wrapper for casting support

*********************************************************
*/
#ifndef __INT_H__
#define __INT_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class IntRep
    : public VarRep
{
public:
    IntRep();
    IntRep(rxInt value);
    IntRep(const IntRep& object);
    virtual ~IntRep();

    rxInt get();
    void set(rxInt value);

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
    rxInt mValue;
};

inline rxInt IntRep::get()
{
    return mValue;
}

inline void IntRep::set(rxInt value)
{
    mValue = value;
}

inline eRefType IntRep::getType()
{
    return eTInt;
}

class Int
    : public Var
{
public:
    Int();
    Int(rxInt value);
    Int(IntRep* rep);
    Int(const Int& toCopy);
    virtual ~Int();

    Int& operator=(const Int& toCopy);
    bool operator==(const Int& toCompare);

    IntRep* operator->() const;
    IntRep* operator*() const;

private:
};

inline Int& Int::operator=(const Int& toCopy)
{
    return (Int&) Ref::operator=(toCopy);
}

inline bool Int::operator==(const Int& toCompare)
{
    return mRep == toCompare.mRep;
}

inline IntRep* Int::operator->() const
{
    return (IntRep*) mRep;
}

inline IntRep* Int::operator*() const
{
    return (IntRep*) mRep;
}

#endif //__INT_H__
