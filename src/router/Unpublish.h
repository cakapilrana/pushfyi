#ifndef __UNPUBLISH_H__
#define __UNPUBLISH_H__

#include <string>
#include "PubSubCmd.h"
#include "Event.h"

class PubSubMaps;

class Unpublish
    : public PubSubCmd
{
public:
    Unpublish(const std::string& topic, const std::string& subtopic, const std::string& device, rxUInt pubId, rxUInt priority = Event::DEFAULT_PRIORITY);
    virtual ~Unpublish();

    virtual void process(PubSubMaps& maps);

    std::string getTopic();
    std::string getSubtopic();
    std::string getDevice();
    rxUInt      getPubId();

private:
    std::string mTopic;
    std::string mSubtopic;
    std::string mDevice;
    rxUInt      mPubId;
};

inline std::string Unpublish::getTopic()
{
    return mTopic;
}

inline std::string Unpublish::getSubtopic()
{
    return mSubtopic;
}

inline std::string Unpublish::getDevice()
{
    return mDevice;
}

inline rxUInt Unpublish::getPubId()
{
    return mPubId;
}

#endif //__UNPUBLISH_H__
