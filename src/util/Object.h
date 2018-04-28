/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: Object

*********************************************************
*/
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include "pushfyi-types.h"
#include "Params.h"
#include "TRef.h"

class ObjectRep
    : public Ref::Rep
{
public:
    ObjectRep();
    ObjectRep(const ObjectRep& object);
    virtual ~ObjectRep();

    // TODO:
    // toString()
    // wait() and notify()

    ObjectRep* clone();

private:
    // Java's Object has a condition variable to provide monitor
    // sections in the code. We can add one here as well.
};

typedef TRef<ObjectRep> Object;

inline ObjectRep* ObjectRep::clone()
{
    return new ObjectRep(*this);
}

#endif //__OBJECT_H__
