/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Regular Expression Exception.

*********************************************************
*/
#include "REException.h"

using namespace std;

REException::REException(const string& message)
    : mMessage(message)
{
    // nop
}

REException::~REException()
{
    // nop
}

