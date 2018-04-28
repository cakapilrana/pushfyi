#include "Unpublish.h"
#include "Event.h"
#include "PubSubMaps.h"

using namespace std;

Unpublish::Unpublish(const string& topic, const string& subtopic, const string& device, rxUInt pubId, rxUInt priority)
    : mTopic(topic), mSubtopic(subtopic), mDevice(device), mPubId(pubId)
{
    mPriority = priority;
}

Unpublish::~Unpublish()
{
    // nop
}

void Unpublish::process(PubSubMaps& maps)
{
    maps.unpublish(this);
}
