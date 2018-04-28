/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Reference counted variables.

*********************************************************
*/
#ifndef __REFVAR_H__
#define __REFVAR_H__

#include <string>
#include "pushfyi-types.h"
#include "Var.h"

class RefVarRep
    : public VarRep
{
public:
    RefVarRep();
    RefVarRep(Ref::Rep* value);
    RefVarRep(const RefVarRep& object);
    virtual ~RefVarRep();

    Ref::Rep* get();
    void set(Ref::Rep* value);

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
    Ref mValue;
};

inline Ref::Rep* RefVarRep::get()
{
    return *mValue;
}

inline void RefVarRep::set(Ref::Rep* value)
{
    mValue = value;
}

inline eRefType RefVarRep::getType()
{
    return eTRefVar;
}

class RefVar
    : public Var
{
public:
    RefVar();
    RefVar(Ref::Rep* value);
    RefVar(RefVarRep* rep);
    RefVar(const RefVar& toCopy);
    virtual ~RefVar();

    RefVar& operator=(const RefVar& toCopy);
    bool operator==(const RefVar& toCompare);

    RefVarRep* operator->() const;
    RefVarRep* operator*() const;

private:
};

inline RefVar& RefVar::operator=(const RefVar& toCopy)
{
    return (RefVar&) Ref::operator=(toCopy);
}

inline bool RefVar::operator==(const RefVar& toCompare)
{
    return mRep == toCompare.mRep;
}

inline RefVarRep* RefVar::operator->() const
{
    return (RefVarRep*) mRep;
}

inline RefVarRep* RefVar::operator*() const
{
    return (RefVarRep*) mRep;
}

#endif //__REFVAR_H__
