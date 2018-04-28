/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: UNSIGNED INT Type Wrapper for casting support

*********************************************************
*/
#include "UInt.h"
#include "STLHelpers.h"

using namespace std;

UIntRep::UIntRep()
    : mValue(0)
{
    // nop
}

UIntRep::UIntRep(rxUInt value)
    : mValue(value)
{
    // nop
}

UIntRep::UIntRep(const UIntRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

UIntRep::~UIntRep()
{
    // nop
}

bool UIntRep::toBool()
{
    return mValue;
}

rxInt UIntRep::toInt()
{
    return mValue;
}

rxUInt UIntRep::toUInt()
{
    return mValue;
}

rxLong UIntRep::toLong()
{
    return mValue;
}

rxULong UIntRep::toULong()
{
    return mValue;
}

rxFloat UIntRep::toFloat()
{
    return mValue;
}

rxDouble UIntRep::toDouble()
{
    return mValue;
}

std::string UIntRep::toString()
{
    return StringUtil::toString(mValue);
}

Ref::Rep* UIntRep::toRep()
{
    return this;
}

void UIntRep::setBool(bool value)
{
    mValue = value;
}

void UIntRep::setInt(rxInt value)
{
    mValue = value;
}

void UIntRep::setUInt(rxUInt value)
{
    mValue = value;
}

void UIntRep::setLong(rxLong value)
{
    mValue = value;
}

void UIntRep::setULong(rxULong value)
{
    mValue = value;
}

void UIntRep::setFloat(rxFloat value)
{
    mValue = (rxUInt) value;
}

void UIntRep::setDouble(rxDouble value)
{
    mValue = (rxUInt) value;
}

void UIntRep::setString(const std::string& value)
{
    mValue = ::strtoul(value.c_str(), 0, 10);
}

void UIntRep::setRep(Ref::Rep*)
{
    // nop
}

UInt::UInt()
    : Var(new UIntRep())
{
    // nop
}

UInt::UInt(UIntRep* rep)
    : Var(rep)
{
    // nop
}

UInt::UInt(const UInt& toCopy)
    : Var(toCopy)
{
    // nop
}

UInt::~UInt()
{
    // nop
}
