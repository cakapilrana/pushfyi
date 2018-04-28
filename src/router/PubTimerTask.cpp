#include "PubTimerTask.h"
#include "PubSubRouter.h"
//#include "util/Log.h"

using namespace std;

PubTimerTaskRep::PubTimerTaskRep()
{
    // nop
}

PubTimerTaskRep::PubTimerTaskRep(Publication* pub, const timeval* expiryTime)
    : TimerCbRep(expiryTime), mPubId(pub->getId()), mTopic(pub->getTopic()), 
      mSubtopic(pub->getSubtopic()), mDevice(pub->getDevice()), mPriority(pub->getPriority())
{
    // nop
}

PubTimerTaskRep::PubTimerTaskRep(PubTimerTaskRep& toCopy)
    : TimerCbRep(toCopy), mPubId(toCopy.mPubId), mTopic(toCopy.mTopic), 
      mSubtopic(toCopy.mSubtopic), mDevice(toCopy.mDevice), mPriority(toCopy.mPriority)
{
    // nop
}

PubTimerTaskRep::~PubTimerTaskRep()
{
    // nop
}

void PubTimerTaskRep::process(timeval* /* tv */)
{
    PubSubRouter::getRouter().unpublish(mTopic, mSubtopic, mDevice, mPubId, mPriority);
}

PubTimerTask::PubTimerTask()
{
    // nop
}

PubTimerTask::PubTimerTask(PubTimerTaskRep* rep)
    : TimerCb(rep)
{
    // nop
}

PubTimerTask::PubTimerTask(const PubTimerTask& toCopy)
    : TimerCb(toCopy)
{
    // nop
}

PubTimerTask::~PubTimerTask()
{
    // nop
}

