/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Base type for ref counted primitives
*********************************************************
*/
#ifndef __VAR_H__
#define __VAR_H__

#include <string>
#include "pushfyi-types.h"
#include "Ref.h"

class VarRep
    : public Ref::Rep
{
public:
    VarRep();
    VarRep(const VarRep& object);
    virtual ~VarRep();

    virtual bool toBool() = 0;
    virtual rxInt toInt() = 0;
    virtual rxUInt toUInt() = 0;
    virtual rxLong toLong() = 0;
    virtual rxULong toULong() = 0;
    virtual rxFloat toFloat() = 0;
    virtual rxDouble toDouble() = 0;
    virtual std::string toString() = 0;
    virtual Ref::Rep* toRep() = 0;

    virtual void setBool(bool value) = 0;
    virtual void setInt(rxInt value) = 0;
    virtual void setUInt(rxUInt value) = 0;
    virtual void setLong(rxLong value) = 0;
    virtual void setULong(rxULong value) = 0;
    virtual void setFloat(rxFloat value) = 0;
    virtual void setDouble(rxDouble value) = 0;
    virtual void setString(const std::string& value) = 0;
    virtual void setRep(Ref::Rep* value) = 0;

private:
};

class Var
    : public Ref
{
public:
    Var();
    Var(VarRep* rep);
    Var(const Var& toCopy);
    virtual ~Var();

    Var& operator=(const Var& toCopy);
    bool operator==(const Var& toCompare) const;

    VarRep* operator->() const;
    VarRep* operator*() const;

private:
};

inline Var& Var::operator=(const Var& toCopy)
{
    return (Var&) Ref::operator=(toCopy);
}

inline bool Var::operator==(const Var& toCompare) const
{
    return mRep == toCompare.mRep;
}

inline VarRep* Var::operator->() const
{
    return (VarRep*) mRep;
}

inline VarRep* Var::operator*() const
{
    return (VarRep*) mRep;
}

#endif //__VAR_H__
