/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: LONG Type Wrapper for casting support

*********************************************************
*/
#include "Long.h"
#include "STLHelpers.h"

using namespace std;

LongRep::LongRep()
    : mValue(0LL)
{
    // nop
}

LongRep::LongRep(rxLong value)
    : mValue(value)
{
    // nop
}

LongRep::LongRep(const LongRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

LongRep::~LongRep()
{
    // nop
}

bool LongRep::toBool()
{
    return mValue;
}

rxInt LongRep::toInt()
{
    return mValue;
}

rxUInt LongRep::toUInt()
{
    return mValue;
}

rxLong LongRep::toLong()
{
    return mValue;
}

rxULong LongRep::toULong()
{
    return mValue;
}

rxFloat LongRep::toFloat()
{
    return mValue;
}

rxDouble LongRep::toDouble()
{
    return mValue;
}

std::string LongRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* LongRep::toRep()
{
    return this;
}

void LongRep::setBool(bool value)
{
    mValue = value;
}

void LongRep::setInt(rxInt value)
{
    mValue = value;
}

void LongRep::setUInt(rxUInt value)
{
    mValue = value;
}

void LongRep::setLong(rxLong value)
{
    mValue = value;
}

void LongRep::setULong(rxULong value)
{
    mValue = value;
}

void LongRep::setFloat(rxFloat value)
{
    mValue = (rxLong) value;
}

void LongRep::setDouble(rxDouble value)
{
    mValue = (rxLong) value;
}

void LongRep::setString(const std::string& value)
{
    mValue = ::strtoll(value.c_str(), 0, 10);
}

void LongRep::setRep(Ref::Rep*)
{
    // nop
}

Long::Long()
    : Var(new LongRep())
{
    // nop
}

Long::Long(rxLong value)
    : Var(new LongRep(value))
{
    // nop
}

Long::Long(LongRep* rep)
    : Var(rep)
{
    // nop
}

Long::Long(const Long& toCopy)
    : Var(toCopy)
{
    // nop
}

Long::~Long()
{
    // nop
}
