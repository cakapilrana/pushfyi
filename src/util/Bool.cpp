/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: BOOL Type Wrapper for casting support

*********************************************************
*/
#include "Bool.h"
#include "STLHelpers.h"

using namespace std;

BoolRep::BoolRep()
    : mValue(false)
{
    // nop
}

BoolRep::BoolRep(bool value)
    : mValue(value)
{
    // nop
}

BoolRep::BoolRep(const BoolRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

BoolRep::~BoolRep()
{
    // nop
}

bool BoolRep::toBool()
{
    return mValue;
}

rxInt BoolRep::toInt()
{
    return mValue;
}

rxUInt BoolRep::toUInt()
{
    return mValue;
}

rxLong BoolRep::toLong()
{
    return mValue;
}

rxULong BoolRep::toULong()
{
    return mValue;
}

rxFloat BoolRep::toFloat()
{
    return mValue;
}

rxDouble BoolRep::toDouble()
{
    return mValue;
}

std::string BoolRep::toString()
{
    return mValue ? string("true") : string("false");
}

Ref::Rep* BoolRep::toRep()
{
    return this;
}

void BoolRep::setBool(bool value)
{
    mValue = value;
}

void BoolRep::setInt(rxInt value)
{
    mValue = value;
}

void BoolRep::setUInt(rxUInt value)
{
    mValue = value;
}

void BoolRep::setLong(rxLong value)
{
    mValue = value;
}

void BoolRep::setULong(rxULong value)
{
    mValue = value;
}

void BoolRep::setFloat(rxFloat value)
{
    mValue = (rxUInt) value;
}

void BoolRep::setDouble(rxDouble value)
{
    mValue = (rxUInt) value;
}

void BoolRep::setString(const std::string& value)
{
    mValue = !(value == "" || value == "0" || value == "false");
}

void BoolRep::setRep(Ref::Rep*)
{
    // nop
}

Bool::Bool()
    : Var(new BoolRep())
{
    // nop
}

Bool::Bool(bool value)
    : Var(new BoolRep(value))
{
    // nop
}

Bool::Bool(BoolRep* rep)
    : Var(rep)
{
    // nop
}

Bool::Bool(const Bool& toCopy)
    : Var(toCopy)
{
    // nop
}

Bool::~Bool()
{
    // nop
}
