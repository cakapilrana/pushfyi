/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: DOUBLE Type Wrapper for casting support

*********************************************************
*/
#ifndef __DOUBLE_H__
#define __DOUBLE_H__

#include <string>
#include <stdlib.h>
#include "pushfyi-types.h"
#include "Var.h"

class DoubleRep
    : public VarRep
{
public:
    DoubleRep();
    DoubleRep(rxDouble value);
    DoubleRep(const DoubleRep& object);
    virtual ~DoubleRep();

    rxDouble get();
    void set(rxDouble value);

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
    rxDouble mValue;
};

inline rxDouble DoubleRep::get()
{
    return mValue;
}

inline void DoubleRep::set(rxDouble value)
{
    mValue = value;
}

inline eRefType DoubleRep::getType()
{
    return eTDouble;
}

class Double
    : public Var
{
public:
    Double();
    Double(rxDouble value);
    Double(DoubleRep* rep);
    Double(const Double& toCopy);
    virtual ~Double();

    Double& operator=(const Double& toCopy);
    bool operator==(const Double& toCompare);

    DoubleRep* operator->() const;
    DoubleRep* operator*() const;

private:
};

inline Double& Double::operator=(const Double& toCopy)
{
    return (Double&) Ref::operator=(toCopy);
}

inline bool Double::operator==(const Double& toCompare)
{
    return mRep == toCompare.mRep;
}

inline DoubleRep* Double::operator->() const
{
    return (DoubleRep*) mRep;
}

inline DoubleRep* Double::operator*() const
{
    return (DoubleRep*) mRep;
}

#endif //__DOUBLE_H__
