/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: UNSIGNED LONG Type Wrapper for casting support

*********************************************************
*/
#include "ULong.h"
#include "STLHelpers.h"

using namespace std;

ULongRep::ULongRep()
    : mValue(0LL)
{
    // nop
}

ULongRep::ULongRep(rxULong value)
    : mValue(value)
{
    // nop
}

ULongRep::ULongRep(const ULongRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

ULongRep::~ULongRep()
{
    // nop
}

bool ULongRep::toBool()
{
    return mValue;
}

rxInt ULongRep::toInt()
{
    return mValue;
}

rxUInt ULongRep::toUInt()
{
    return mValue;
}

rxLong ULongRep::toLong()
{
    return mValue;
}

rxULong ULongRep::toULong()
{
    return mValue;
}

rxFloat ULongRep::toFloat()
{
    return mValue;
}

rxDouble ULongRep::toDouble()
{
    return mValue;
}

std::string ULongRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* ULongRep::toRep()
{
    return this;
}

void ULongRep::setBool(bool value)
{
    mValue = value;
}

void ULongRep::setInt(rxInt value)
{
    mValue = value;
}

void ULongRep::setUInt(rxUInt value)
{
    mValue = value;
}

void ULongRep::setLong(rxLong value)
{
    mValue = value;
}

void ULongRep::setULong(rxULong value)
{
    mValue = value;
}

void ULongRep::setFloat(rxFloat value)
{
    mValue = (rxULong) value;
}

void ULongRep::setDouble(rxDouble value)
{
    mValue = (rxULong) value;
}

void ULongRep::setString(const std::string& value)
{
    mValue = ::strtoull(value.c_str(), 0, 10);
}

void ULongRep::setRep(Ref::Rep*)
{
    // nop
}

ULong::ULong()
    : Var(new ULongRep())
{
    // nop
}

ULong::ULong(rxULong value)
    : Var(new ULongRep(value))
{
    // nop
}

ULong::ULong(ULongRep* rep)
    : Var(rep)
{
    // nop
}

ULong::ULong(const ULong& toCopy)
    : Var(toCopy)
{
    // nop
}

ULong::~ULong()
{
    // nop
}
