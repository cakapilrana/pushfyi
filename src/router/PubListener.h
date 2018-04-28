#ifndef __PUBLISTENER_H__
#define __PUBLISTENER_H__

#include "Event.h"

class PubListener
{
public:
    virtual ~PubListener(){}
    virtual bool publishing(const Event& event) = 0;
private:
};

#endif //__PUBLISTENER_H__
