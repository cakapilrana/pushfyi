/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: String Type Wrapper for casting support

*********************************************************
*/
#include "String.h"
#include "STLHelpers.h"

using namespace std;

StringRep::StringRep()
{
    // nop
}

StringRep::StringRep(const string& value)
    : mValue(value)
{
    // nop
}

StringRep::StringRep(const StringRep& toCopy)
    : VarRep(toCopy), mValue(toCopy.mValue)
{
    // nop
}

StringRep::~StringRep()
{
    // nop
}

bool StringRep::toBool()
{
    return !(mValue == "" || mValue == "0" || mValue == "false");
}

rxInt StringRep::toInt()
{
    return ::atoi(mValue.c_str());
}

rxUInt StringRep::toUInt()
{
    return ::strtoul(mValue.c_str(), 0, 10);
}

rxLong StringRep::toLong()
{
    return ::strtoll(mValue.c_str(), 0, 10);
}

rxULong StringRep::toULong()
{
    return ::strtoull(mValue.c_str(), 0, 10);
}

rxFloat StringRep::toFloat()
{
    return ::strtof(mValue.c_str(), 0);
}

rxDouble StringRep::toDouble()
{
    return ::strtod(mValue.c_str(), 0);
}

std::string StringRep::toString()
{
    return mValue;
}

Ref::Rep* StringRep::toRep()
{
    return this;
}

void StringRep::setBool(bool value)
{
    mValue = value ? string("true") : string("false");
}

void StringRep::setInt(rxInt value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setUInt(rxUInt value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setLong(rxLong value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setULong(rxULong value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setFloat(rxFloat value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setDouble(rxDouble value)
{
    mValue = StringUtil::toString(value);
}

void StringRep::setString(const std::string& value)
{
    mValue = value;
}

void StringRep::setRep(Ref::Rep*)
{
    // nop
}

String::String()
    : Var(new StringRep())
{
    // nop
}

String::String(const string& value)
    : Var(new StringRep(value))
{
    // nop
}

String::String(StringRep* rep)
    : Var(rep)
{
    // nop
}

String::String(const String& toCopy)
    : Var(toCopy)
{
    // nop
}

String::~String()
{
    // nop
}
