/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Template type for reference counting
*********************************************************
*/
#ifndef __TREF_H__
#define __TREF_H__

#include "Ref.h"

template<class T>
class TRef
    : public Ref
{
public:
    TRef();
    TRef(T* rep);
    TRef(const TRef& toCopy);
    virtual ~TRef();

    TRef& operator=(const TRef& toCopy);
    bool operator==(const TRef& toCompare);

    T* operator->() const;
    T* operator*() const;

private:
};

template<class T>
TRef<T>::TRef()
    : Ref(new T)
{
    // nop
}

template<class T>
TRef<T>::TRef(T* rep)
    : Ref(rep)
{
    // nop
}

template<class T>
TRef<T>::TRef(const TRef& toCopy)
    : Ref(toCopy)
{
    // nop
}

template<class T>
TRef<T>::~TRef()
{
    // nop
}

template<class T>
TRef<T>& TRef<T>::operator=(const TRef<T>& toCopy)
{
    release();
    assign(toCopy.mRep);

    return (*this);
}

template<class T>
bool TRef<T>::operator==(const TRef<T>& toCompare)
{
    return mRep == toCompare.mRep;
}

template<class T>
T* TRef<T>::operator->() const
{
    return (T*) mRep;
}

template<class T>
T* TRef<T>::operator*() const
{
    return (T*) mRep;
}

#endif //__TREF_H__
