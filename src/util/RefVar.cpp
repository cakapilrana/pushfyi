/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Reference counted variables.

*********************************************************
*/
#include "RefVar.h"
#include "STLHelpers.h"

using namespace std;

RefVarRep::RefVarRep()
    : mValue(0)
{
    // nop
}

RefVarRep::RefVarRep(Ref::Rep* value)
    : mValue(value)
{
    // nop
}

RefVarRep::RefVarRep(const RefVarRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

RefVarRep::~RefVarRep()
{
    // nop
}

bool RefVarRep::toBool()
{
    return *mValue;
}

rxInt RefVarRep::toInt()
{
    //return (rxInt) *mValue;
    return *((rxInt*)(&mValue));
}

rxUInt RefVarRep::toUInt()
{
    //return (rxUInt) *mValue;
    return *((rxUInt*)(&mValue));
}

rxLong RefVarRep::toLong()
{
    //return (rxLong) (rxUInt) *mValue;
    return (rxLong) *((rxUInt*)(&mValue));
}

rxULong RefVarRep::toULong()
{
    //return (rxULong) (rxUInt) *mValue;
    return (rxULong) *((rxUInt*)(&mValue));
}

rxFloat RefVarRep::toFloat()
{
    //return (rxFloat) (rxUInt) *mValue;
    return (rxFloat) *((rxUInt*)(&mValue));
}

rxDouble RefVarRep::toDouble()
{
    //return (rxDouble) (rxUInt) *mValue;
    return (rxDouble) *((rxUInt*)(&mValue));
}

std::string RefVarRep::toString()
{
    return !mValue.isNull() ? mValue->toString() : "";
}

Ref::Rep* RefVarRep::toRep()
{
    return *mValue;
}

void RefVarRep::setBool(bool)
{
    // nop
}

void RefVarRep::setInt(rxInt)
{
    // nop
}

void RefVarRep::setUInt(rxUInt)
{
    // nop
}

void RefVarRep::setLong(rxLong)
{
    // nop
}

void RefVarRep::setULong(rxULong)
{
    // nop
}

void RefVarRep::setFloat(rxFloat)
{
    // nop
}

void RefVarRep::setDouble(rxDouble)
{
    // nop
}

void RefVarRep::setString(const std::string&)
{
    // nop
}

void RefVarRep::setRep(Ref::Rep* value)
{
    mValue = value;
}

RefVar::RefVar()
    : Var(new RefVarRep())
{
    // nop
}

RefVar::RefVar(Ref::Rep* value)
    : Var(new RefVarRep(value))
{
    // nop
}

RefVar::RefVar(RefVarRep* rep)
    : Var(rep)
{
    // nop
}

RefVar::RefVar(const RefVar& toCopy)
    : Var(toCopy)
{
    // nop
}

RefVar::~RefVar()
{
    // nop
}

