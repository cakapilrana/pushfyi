/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Reference counting implementation.
Saves a lot of memory for Event objects.

*********************************************************
*/
#include "Ref.h"

Mutex Ref::mMutex;

Ref::Ref()
    : mRep(0)
{
    // nop
}

Ref::Ref(Rep* rep)
{
    assign(rep);
}

Ref::Ref(const Ref& toCopy)
{
    assign(toCopy.mRep);
}

Ref::~Ref()
{
    release();
}

Ref::Rep::Rep()
    : mRefCount(0)
{
    // nop
}

Ref::Rep::Rep(const Rep& /*toCopy*/)
    : mRefCount(0)
{
    // nop
}

Ref::Rep::~Rep()
{
    // nop
}

