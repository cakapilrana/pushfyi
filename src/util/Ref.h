/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Reference counting implementation.
Saves a lot of memory for Event objects.

*********************************************************
*/
#ifndef __REF_H__
#define __REF_H__

#include <string>
#include "Mutex.h"
#include "Synchronized.h"
#include "RefTypes.h"
#include "pushfyi-types.h"

class Ref
{
public:
    class Rep
    {

    public:
        Rep();
        Rep(const Rep& rep);
        virtual ~Rep();

        rxUInt mRefCount;

        virtual eRefType getType();
        virtual std::string toString();
    };

    Ref();
    Ref(Rep* rep);
    Ref(const Ref& toCopy);
    virtual ~Ref();

    Ref& operator=(const Ref& toCopy);

    Rep* operator->() const;
    Rep* operator*() const;

    bool isNull() const;
    Rep* detach();

protected:
    static Mutex mMutex;

    void release();
    void assign(Rep* rep);

    Rep* mRep;
};

inline eRefType Ref::Rep::getType()
{
    return eTUnknown;
}

inline std::string Ref::Rep::toString()
{
    return "";
}

inline Ref& Ref::operator=(const Ref& toCopy)
{
    release();
    assign(toCopy.mRep);

    return (*this);
}

inline Ref::Rep* Ref::operator->() const
{
    return mRep;
}

inline Ref::Rep* Ref::operator*() const
{
    return mRep;
}

inline bool Ref::isNull() const
{
    return mRep == NULL;
}

inline Ref::Rep* Ref::detach()
{
    Synchronized lock(mMutex);

    if (mRep != NULL) {
        mRep->mRefCount++;
    }

    return mRep;
}

inline void Ref::release()
{
    Rep* toFree = NULL;

    { Synchronized lock(mMutex);

        if (mRep != NULL) {
            if (--mRep->mRefCount <= 0) {
                toFree = mRep;
                mRep   = NULL;
            }
        }
    }

    if (toFree != NULL) {
        delete toFree;
    }
}

inline void Ref::assign(Rep* rep)
{
    Synchronized lock(mMutex);

    mRep = rep;
    if (mRep != NULL) {
        mRep->mRefCount++;
    }
}

#endif //__REF_H__
