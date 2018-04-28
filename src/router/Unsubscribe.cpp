#include "Unsubscribe.h"
#include "PubSubMaps.h"
//#include "util/Log.h"

using namespace std;

Unsubscribe::Unsubscribe(const string&    topic, 
                         const string&    subtopic, 
                         const string&    context, 
                         const string&    subcontext, 
                         Pushable<Event>* pushable, 
                         bool             isTimeout, 
                         rxUInt           priority)
    : mTopic(topic), mSubtopic(subtopic), mContext(context), mSubcontext(subcontext),
      mPushable(pushable), mIsTimeout(isTimeout), mIsProcessed(false)
{
    mPriority = priority;
}

Unsubscribe::~Unsubscribe()
{
    // nop
}

void Unsubscribe::process(PubSubMaps& maps)
{
    maps.unsubscribe(this);
}

bool Unsubscribe::equals(Subscription* toCompare)
{
    return mContext    == toCompare->getContext()    && 
           mSubcontext == toCompare->getSubcontext() && 
           mPushable   == toCompare->getPushable();
}

