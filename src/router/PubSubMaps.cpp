#include <iostream>
#include "PubSubMaps.h"
#include "AndOrSubscription.h"
#include "AndOrUnsubscribe.h"
#include "Publication.h"
//#include "PubSubRouter.h"
#include "Unpublish.h"
#include "Unsubscribe.h"
#include "Log.h"

using namespace std;

PubSubMaps::PubSubMaps()
{
    // nop
}

PubSubMaps::~PubSubMaps()
{
    // nop
}

void PubSubMaps::subscribe(Subscription* sub)
{
    string topic      = sub->getTopic();
    string subtopic   = sub->getSubtopic();
    string context    = sub->getContext();
    string subcontext = sub->getSubcontext();
    bool   subscribed = false;

/*
    INFO(Log::ePubSub, "Subscribe[%u,%s,%s], Context[%s,%s]", sub->getId(), 
          topic.c_str(), subtopic.c_str(), context.c_str(), subcontext.c_str());
*/

    if (isWildcard(topic)) {
        subscribed = get(mTopicMap, subtopic)->subscribe(sub);
    }
    else if (isWildcard(subtopic)) {
        subscribed = get(mSubtopicMap, topic)->subscribe(sub);
    }
    else {
        subscribed = get(topic, subtopic)->subscribe(sub);
    }

    if (subscribed) {
        sub->scheduleTimeout();
    }
    else {
        delete sub;
    }
}

void PubSubMaps::subscribe(AndOrSubscription* sub)
{
    vector<string>&    topics       = *sub->getTopics();
    string             topicExpr    = sub->getTopicExpression();
    string             subtopic     = sub->getSubtopic();
    string             context      = sub->getContext();
    string             subcontext   = sub->getSubcontext();
    bool               isPersistent = sub->isPersistent();
    PubSubMap&         map          = isWildcard(subtopic) ? mSubtopicMap : *getMap(subtopic);
    bool               subscribed   = false;
    AndOrSubscription* root         = 0;

/*
    INFO(Log::ePubSub, "Subscription[%u,%s,%s], Context[%s,%s]", sub->getId(), 
          topicExpr.c_str(), subtopic.c_str(), context.c_str(), subcontext.c_str());
*/

    // Must create subscription list first to ensure isMatched()
    // for AND subscriptions performs properly in the next loop
    for (rxUInt i = 0; i < topics.size(); i++) {
        sub->createChild(topics[i], root);
    }

    AndOrSubscription* child = root;

    do {
        AndOrSubscription* next  = child->nextSibling();
        string             topic = child->getTopic();

        if (!get(map, topic)->subscribe(child)) {
            //AndOrSubscription should be done as atomic action
            //so abort should rollback all handled child subs
            child->remove();
            cleanup(map, child);
            delete child;
            break; // ABORT SUBSCRIBE
        }
        else {
            subscribed = true;

            if (child->isMatched() && !isPersistent) {
                cleanup(map, sub);
                delete sub;
                return; // DONE: Matched subscription and it's not persistent
            }

            sub->scheduleTimeout(child);
            child = next;
        }
    } while (child != root);

    delete sub;
}

void PubSubMaps::unsubscribe(Unsubscribe* unsub)
{
    string topic      = unsub->getTopic();
    string subtopic   = unsub->getSubtopic();
    string context    = unsub->getContext();
    string subcontext = unsub->getSubcontext();

/*
    TRACE(PubSubRouter::mTraceId, "Unsubscribe[%u,%s,%s], Context[%s,%s]", unsub->getId(), 
          topic.c_str(), subtopic.c_str(), context.c_str(), subcontext.c_str());
*/

    if (isWildcard(topic)) {
        unsubscribe(mTopicMap, subtopic, unsub);
    }
    else if (isWildcard(subtopic)) {
        unsubscribe(mSubtopicMap, topic, unsub);
    }
    else {
        unsubscribe(mMap, topic, subtopic, unsub);
    }

    delete unsub;
}

void PubSubMaps::unsubscribe(AndOrUnsubscribe* unsub)
{
    vector<string>& topics     = *unsub->getTopics();
    string          topicExpr  = unsub->getTopicExpression();
    string          subtopic   = unsub->getSubtopic();
    string          context    = unsub->getContext();
    string          subcontext = unsub->getSubcontext();

/*
    INFO(Log::ePubSub, "Unsubscribe[%u,%s,%s], Context[%s,%s]", unsub->getId(), 
          topicExpr.c_str(), subtopic.c_str(), context.c_str(), subcontext.c_str());
*/

    if (isWildcard(subtopic)) {
        for (rxUInt i = 0; i < topics.size(); i++) {
            unsubscribe(mSubtopicMap, topics[i], unsub);
        }
    }
    else {
        for (rxUInt i = 0; i < topics.size(); i++) {
            unsubscribe(mMap, topics[i], subtopic, unsub);
        }
    }

    delete unsub;
}

bool PubSubMaps::unsubscribe(PubSubMap& map, const string& key, Unsubscribe* unsub)
{
    PubSubMapIter iter = map.find(key);

    if (iter != map.end()) {
        if (iter->second->unsubscribe(unsub)) {
            delete iter->second;
            map.erase(iter);
        }
    }

    return map.empty();
}

bool PubSubMaps::unsubscribe(PubSubMapMap& map, const string& topic, const string& subtopic, Unsubscribe* unsub)
{
    PubSubMapMapIter iter = map.find(subtopic);

    if (iter != map.end()) {
        if (unsubscribe(*iter->second, topic, unsub)) {
            delete iter->second;
            map.erase(iter);
        }
    }

    return map.empty();
}

void PubSubMaps::publish(Publication* pub)
{
    string topic    = pub->getTopic();
    string subtopic = pub->getSubtopic();
    string device   = pub->getDevice();
    bool   hasTTL   = pub->hasTTL();

    if (device.size() == 0) {
        publish(topic, subtopic, pub);
    }
    else {
        Event        event1 = pub->getEvent();
        Event        event2 = event1->clone();
        Publication* pub2   = new Publication(pub->getId(), event2, pub->getTTL());

        // Publish topic=classification, subtopic=device
        event1->setSubtopic(device);
        publish(topic, device, pub);

        // Publish topic=tagId, subtopic=device
        event2->setTopic(subtopic);
        event2->setSubtopic(device);
        publish(subtopic, device, pub2);
    }

    if (hasTTL) {
        pub->scheduleTimeout();
    }
}

void PubSubMaps::publish(const string& topic, const string& subtopic, Publication* pub)
{

/*
    INFO(Log::ePubSub, "Publish[%u,%s,%s]", pub->getId(), 
          topic.c_str(), subtopic.c_str());
*/

    if (pub->hasTTL()) {
        get(mTopicMap, subtopic)->publish(pub);
        get(mSubtopicMap, topic)->publish(pub);
        get(topic, subtopic)->publish(pub);
    }
    else {
        PubSubMapIter    tIter = mTopicMap.find(subtopic);
        PubSubMapIter    sIter = mSubtopicMap.find(topic);
        PubSubMapMapIter mIter = mMap.find(subtopic);

        if (tIter != mTopicMap.end()) {
            if (tIter->second->publish(pub)) {
                delete tIter->second;
                mTopicMap.erase(tIter);
            }
        }

        if (sIter != mSubtopicMap.end()) {
            if (sIter->second->publish(pub)) {
                delete sIter->second;
                mSubtopicMap.erase(sIter);
            }
        }

        if (mIter != mMap.end()) {
            PubSubMap*    map  = mIter->second;
            PubSubMapIter iter = map->find(topic);

            if (iter != map->end()) {
                if (iter->second->publish(pub)) {
                    delete iter->second;
                    map->erase(iter);

                    if (map->empty()) {
                        delete map;
                        mMap.erase(mIter);
                    }
                }
            }
        }

        delete pub;
    }
}

void PubSubMaps::unpublish(Unpublish* unpub)
{
    string topic    = unpub->getTopic();
    string subtopic = unpub->getSubtopic();
    string device   = unpub->getDevice();

    if (device.size() == 0) {
        unpublish(topic, subtopic, unpub);
    }
    else {
        // Unpublish topic=classification, subtopic=device
        unpublish(topic, device, unpub);

        // Unpublish topic=tagId, subtopic=device
        unpublish(subtopic, device, unpub);
    }

    delete unpub;
}

void PubSubMaps::unpublish(const string& topic, const string& subtopic, Unpublish* unpub)
{
/*
    INFO(Log::ePubSub, "Unpublish[%u,%s,%s]", unpub->getId(), 
          topic.c_str(), subtopic.c_str());
*/

    PubSubMapIter    tIter = mTopicMap.find(subtopic);
    PubSubMapIter    sIter = mSubtopicMap.find(topic);
    PubSubMapMapIter mIter = mMap.find(subtopic);

    if (tIter != mTopicMap.end()) {
        if (tIter->second->unpublish(unpub)) {
            delete tIter->second;
            mTopicMap.erase(tIter);
        }
    }

    if (sIter != mSubtopicMap.end()) {
        if (sIter->second->unpublish(unpub)) {
            delete sIter->second;
            mSubtopicMap.erase(sIter);
        }
    }

    if (mIter != mMap.end()) {
        PubSubMap*    map  = mIter->second;
        PubSubMapIter iter = map->find(topic);

        if (iter != map->end()) {
            if (iter->second->unpublish(unpub)) {
                delete iter->second;
                map->erase(iter);

                if (map->empty()) {
                    delete map;
                    mMap.erase(mIter);
                }
            }
        }
    }
}

void PubSubMaps::cleanup(Subscription* subscription)
{
    if (subscription->getOperator() != '|') {
        return; // nop: cleanup only required for OR subscriptions
    }

    AndOrSubscription* sub = (AndOrSubscription*) subscription;

    for (AndOrSubscription* cur = sub->nextSibling(); cur != sub; cur = cur->nextSibling()) {
        if (!cur->Subscription::isMatched()) {
            PubSubMapMapIter mIter = mMap.find(cur->getSubtopic());

            if (mIter != mMap.end()) {
                PubSubMap*    map  = mIter->second;
                PubSubMapIter iter = map->find(cur->getTopic());

                if (iter != map->end() && iter->second->isEmpty()) {
                    delete iter->second;
                    map->erase(iter);

                    if (map->empty()) {
                        delete map;
                        mMap.erase(mIter);
                    }
                }
            }
        }
    }
}

void PubSubMaps::cleanup(PubSubMap& map, AndOrSubscription* sub)
{
    if (sub->getOperator() != '|') {
        return; // nop: cleanup only required for OR subscriptions
    }

    for (AndOrSubscription* cur = sub->nextSibling(); cur != sub; cur = cur->nextSibling()) {
        if (!cur->Subscription::isMatched()) {
            PubSubMapIter iter  = map.find(cur->getTopic());

            if (iter != map.end() && iter->second->isEmpty()) {
                delete iter->second;
                map.erase(iter);
            }
        }
    }
}

string PubSubMaps::toString(PubSubMap* map)
{
    string result;
    for (PubSubMap::iterator it = map->begin(); it != map->end(); it++) {
        result += "    "+ it->first + "\n" + it->second->toString();
    }
    return result;
}

string PubSubMaps::toString()
{
    string result;

    result += "TOPIC WILDCARDS\n";
    result += toString(&mTopicMap);

    result += "\nSUBTOPIC WILDCARDS\n";
    result += toString(&mSubtopicMap);

    result += "\nMAP\n";

    for (PubSubMapMapIter iter = mMap.begin(); iter != mMap.end(); iter++) {
        result += iter->first + "\n";
        result += toString(iter->second);
    }

    return result;
}

rxUInt PubSubMaps::size()
{
    rxUInt sum = 0;
    PubSubMapMap::iterator it = mMap.begin();
    for (; it != mMap.end(); it++){
        sum += it->second->size();
    }
    return sum + mTopicMap.size() + mSubtopicMap.size();
}
