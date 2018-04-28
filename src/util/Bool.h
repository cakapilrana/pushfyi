/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: BOOL Type Wrapper for casting support

*********************************************************
*/
#ifndef __BOOL_H__
#define __BOOL_H__

#include <string>
#include "pushfyi-types.h"
#include "Var.h"

class BoolRep
    : public VarRep
{
public:
    BoolRep();
    BoolRep(bool value);
    BoolRep(const BoolRep& object);
    virtual ~BoolRep();

    bool get();
    void set(bool value);

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
    // Intentionally using rxUInt to provide toInt()
    // and toUInt() without loss of information
    rxUInt mValue;
};

inline bool BoolRep::get()
{
    return mValue;
}

inline void BoolRep::set(bool value)
{
    mValue = value;
}

inline eRefType BoolRep::getType()
{
    return eTBool;
}

class Bool
    : public Var
{
public:
    Bool();
    Bool(bool value);
    Bool(BoolRep* rep);
    Bool(const Bool& toCopy);
    virtual ~Bool();

    Bool& operator=(const Bool& toCopy);
    bool operator==(const Bool& toCompare);

    BoolRep* operator->() const;
    BoolRep* operator*() const;

private:
};

inline Bool& Bool::operator=(const Bool& toCopy)
{
    return (Bool&) Ref::operator=(toCopy);
}

inline bool Bool::operator==(const Bool& toCompare)
{
    return mRep == toCompare.mRep;
}

inline BoolRep* Bool::operator->() const
{
    return (BoolRep*) mRep;
}

inline BoolRep* Bool::operator*() const
{
    return (BoolRep*) mRep;
}

#endif //__BOOL_H__
