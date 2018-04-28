/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: Base type for ref counted primitives
*********************************************************
*/
#include "Var.h"

using namespace std;
VarRep::VarRep()
{
    // nop
}

VarRep::VarRep(const VarRep& /* object */)
    : Ref::Rep()
{
    // nop
}

VarRep::~VarRep()
{
    // nop
}

Var::Var()
{
    // nop
}

Var::Var(VarRep* rep)
    : Ref(rep)
{
    // nop
}

Var::Var(const Var& toCopy)
    : Ref(toCopy)
{
    // nop
}

Var::~Var()
{
    // nop
}

