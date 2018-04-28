#ifndef __PUBSUBCMD_H__
#define __PUBSUBCMD_H__

#include "Mutex.h"
#include "pushfyi-types.h"
#include "Synchronized.h"

class PubSubMaps;
class Subscription;

class PubSubCmd
{
public:
    PubSubCmd();
    PubSubCmd(rxUInt id, rxUInt priority);
    PubSubCmd(PubSubCmd* cmd);
    virtual ~PubSubCmd();

    virtual void process(PubSubMaps& maps) = 0;

    rxUInt getId();

    rxUInt getPriority();
    void setPriority(rxUInt priority);

    void addSub(Subscription* sub);
    Subscription* nextSub();

protected:
    static rxUInt generateId();

    static rxUInt UNIQUE_ID;
    static Mutex  MUTEX;

    rxUInt        mId;
    rxUInt        mPriority;
    Subscription* mSub;

private:
};

inline rxUInt PubSubCmd::getId()
{
    return mId;
}

inline rxUInt PubSubCmd::getPriority()
{
    return mPriority;
}

inline void PubSubCmd::setPriority(rxUInt priority)
{
    mPriority = priority;
}

inline Subscription* PubSubCmd::nextSub()
{
    return mSub;
}

inline rxUInt PubSubCmd::generateId()
{
    Synchronized lock(MUTEX);

    if (UNIQUE_ID == 0) {
        UNIQUE_ID++;
    }

    return UNIQUE_ID++;
}


#endif //__PUBSUBCMD_H__
