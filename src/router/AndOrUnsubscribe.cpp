#include "AndOrUnsubscribe.h"
#include "AndSubscription.h"
#include "PubSubMaps.h"

using namespace std;

AndOrUnsubscribe::AndOrUnsubscribe(vector<string>*  topics, 
                                   const string&    subtopic, 
                                   const string&    context, 
                                   const string&    subcontext, 
                                   Pushable<Event>* pushable, 
                                   bool             isTimeout, 
                                   rxUInt           priority)
    : Unsubscribe(RX_ESREF, subtopic, context, subcontext, pushable, isTimeout, priority), mTopics(*topics)
{
    // nop
}

AndOrUnsubscribe::~AndOrUnsubscribe()
{
    // nop
}

void AndOrUnsubscribe::process(PubSubMaps& maps)
{
    maps.unsubscribe(this);
}

