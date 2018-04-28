/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: INT Type Wrapper for casting support

*********************************************************
*/
#include "Int.h"
#include "STLHelpers.h"

using namespace std;

IntRep::IntRep()
    : mValue(0)
{
    // nop
}

IntRep::IntRep(rxInt value)
    : mValue(value)
{
    // nop
}

IntRep::IntRep(const IntRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

IntRep::~IntRep()
{
    // nop
}

bool IntRep::toBool()
{
    return mValue;
}

rxInt IntRep::toInt()
{
    return mValue;
}

rxUInt IntRep::toUInt()
{
    return mValue;
}

rxLong IntRep::toLong()
{
    return mValue;
}

rxULong IntRep::toULong()
{
    return mValue;
}

rxFloat IntRep::toFloat()
{
    return (rxFloat) mValue;
}

rxDouble IntRep::toDouble()
{
    return (rxDouble) mValue;
}

std::string IntRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* IntRep::toRep()
{
    return this;
}

void IntRep::setBool(bool value)
{
    mValue = value;
}

void IntRep::setInt(rxInt value)
{
    mValue = value;
}

void IntRep::setUInt(rxUInt value)
{
    mValue = value;
}

void IntRep::setLong(rxLong value)
{
    mValue = value;
}

void IntRep::setULong(rxULong value)
{
    mValue = value;
}

void IntRep::setFloat(rxFloat value)
{
    mValue = (rxInt) value;
}

void IntRep::setDouble(rxDouble value)
{
    mValue = (rxInt) value;
}

void IntRep::setString(const std::string& value)
{
    mValue = ::atoi(value.c_str());
}

void IntRep::setRep(Ref::Rep*)
{
    // nop
}

Int::Int()
    : Var(new IntRep())
{
    // nop
}

Int::Int(rxInt value)
    : Var(new IntRep(value))
{
    // nop
}

Int::Int(IntRep* rep)
    : Var(rep)
{
    // nop
}

Int::Int(const Int& toCopy)
    : Var(toCopy)
{
    // nop
}

Int::~Int()
{
    // nop
}
