/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: DOUBLE Type Wrapper for casting support

*********************************************************
*/
#include "Double.h"
#include "STLHelpers.h"

using namespace std;

DoubleRep::DoubleRep()
    : mValue(0.0)
{
    // nop
}

DoubleRep::DoubleRep(rxDouble value)
    : mValue(value)
{
    // nop
}

DoubleRep::DoubleRep(const DoubleRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

DoubleRep::~DoubleRep()
{
    // nop
}

bool DoubleRep::toBool()
{
    return mValue;
}

rxInt DoubleRep::toInt()
{
    return (rxInt) mValue;
}

rxUInt DoubleRep::toUInt()
{
    return (rxUInt) mValue;
}

rxLong DoubleRep::toLong()
{
    return (rxLong) mValue;
}

rxULong DoubleRep::toULong()
{
    return (rxULong) mValue;
}

rxFloat DoubleRep::toFloat()
{
    return (rxFloat) mValue;
}

rxDouble DoubleRep::toDouble()
{
    return mValue;
}

std::string DoubleRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* DoubleRep::toRep()
{
    return this;
}

void DoubleRep::setBool(bool value)
{
    mValue = value;
}

void DoubleRep::setInt(rxInt value)
{
    mValue = value;
}

void DoubleRep::setUInt(rxUInt value)
{
    mValue = value;
}

void DoubleRep::setLong(rxLong value)
{
    mValue = value;
}

void DoubleRep::setULong(rxULong value)
{
    mValue = value;
}

void DoubleRep::setFloat(rxFloat value)
{
    mValue = value;
}

void DoubleRep::setDouble(rxDouble value)
{
    mValue = value;
}

void DoubleRep::setString(const std::string& value)
{
    mValue = ::strtod(value.c_str(), 0);
}

void DoubleRep::setRep(Ref::Rep*)
{
    // nop
}

Double::Double()
    : Var(new DoubleRep())
{
    // nop
}

Double::Double(rxDouble value)
    : Var(new DoubleRep(value))
{
    // nop
}

Double::Double(DoubleRep* rep)
    : Var(rep)
{
    // nop
}

Double::Double(const Double& toCopy)
    : Var(toCopy)
{
    // nop
}

Double::~Double()
{
    // nop
}

