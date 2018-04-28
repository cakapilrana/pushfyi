/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: FLOAT Type Wrapper for casting support

*********************************************************
*/
#include "Float.h"
#include "STLHelpers.h"

using namespace std;

FloatRep::FloatRep()
    : mValue(0.0)
{
    // nop
}

FloatRep::FloatRep(rxFloat value)
    : mValue(value)
{
    // nop
}

FloatRep::FloatRep(const FloatRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

FloatRep::~FloatRep()
{
    // nop
}

bool FloatRep::toBool()
{
    return mValue;
}

rxInt FloatRep::toInt()
{
    return (rxInt) mValue;
}

rxUInt FloatRep::toUInt()
{
    return (rxUInt) mValue;
}

rxLong FloatRep::toLong()
{
    return (rxLong) mValue;
}

rxULong FloatRep::toULong()
{
    return (rxULong) mValue;
}

rxFloat FloatRep::toFloat()
{
    return mValue;
}

rxDouble FloatRep::toDouble()
{
    return (rxDouble) mValue;
}

std::string FloatRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* FloatRep::toRep()
{
    return this;
}

void FloatRep::setBool(bool value)
{
    mValue = value;
}

void FloatRep::setInt(rxInt value)
{
    mValue = value;
}

void FloatRep::setUInt(rxUInt value)
{
    mValue = value;
}

void FloatRep::setLong(rxLong value)
{
    mValue = value;
}

void FloatRep::setULong(rxULong value)
{
    mValue = value;
}

void FloatRep::setFloat(rxFloat value)
{
    mValue = value;
}

void FloatRep::setDouble(rxDouble value)
{
    mValue = value;
}

void FloatRep::setString(const std::string& value)
{
    mValue = ::strtof(value.c_str(), 0);
}

void FloatRep::setRep(Ref::Rep*)
{
    // nop
}

Float::Float()
    : Var(new FloatRep())
{
    // nop
}

Float::Float(rxFloat value)
    : Var(new FloatRep(value))
{
    // nop
}

Float::Float(FloatRep* rep)
    : Var(rep)
{
    // nop
}

Float::Float(const Float& toCopy)
    : Var(toCopy)
{
    // nop
}

Float::~Float()
{
    // nop
}
