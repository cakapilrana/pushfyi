/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: JAVA like Runnable interface for using threads.

*********************************************************
*/
#ifndef __RUNNABLE_H__
#define __RUNNABLE_H__

class Runnable
{
public:
    Runnable();
    virtual ~Runnable();

    virtual void run();
};

#endif //__RUNNABLE_H__
