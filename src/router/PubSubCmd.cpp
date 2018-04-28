#include "PubSubCmd.h"
#include "Subscription.h"

using namespace std;

rxUInt PubSubCmd::UNIQUE_ID = 0;
Mutex  PubSubCmd::MUTEX;

PubSubCmd::PubSubCmd()
    : mId(generateId()), mPriority(Event::DEFAULT_PRIORITY), mSub(0)
{
    // nop
}

PubSubCmd::PubSubCmd(rxUInt id, rxUInt priority)
    : mId(id), mPriority(priority), mSub(0)
{
    // nop
}

PubSubCmd::PubSubCmd(PubSubCmd* cmd)
    : mId(generateId()), mPriority(cmd->mPriority), mSub(cmd->mSub)
{
    // nop
}

PubSubCmd::~PubSubCmd()
{
    // nop
}

void PubSubCmd::addSub(Subscription* toAdd)
{
    toAdd->mSub = mSub;
    mSub        = toAdd;
}

