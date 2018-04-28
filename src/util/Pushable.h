/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Interface for receiving Events from Queue.
Class implementing this interface must define it's own
Queue and how it wants to handle Events.

*********************************************************
*/
#ifndef __PUSHABLE_H__
#define __PUSHABLE_H__

template <class T> class Pushable
{
public:
    Pushable() {};
    virtual ~Pushable() {};

    virtual void push(T t) = 0;
    virtual void pushFront(T t) = 0;

private:
};

#endif //__PUSHABLE_H__
