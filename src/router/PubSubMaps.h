#ifndef __PUBSUBMAPS_H__
#define __PUBSUBMAPS_H__

#include <string>
//#include <map>
#include <ext/hash_map>
#include "PubSubEntry.h"
#include "STLHelpers.h"

class AndOrSubscription;
class AndOrUnsubscribe;

class PubSubMaps
{
public:
    PubSubMaps();
    ~PubSubMaps();

    void subscribe(Subscription* sub);
    void subscribe(AndOrSubscription* sub);

    void unsubscribe(Unsubscribe* unsub);
    void unsubscribe(AndOrUnsubscribe* unsub);

    void publish(Publication* pub);
    void unpublish(Unpublish* unpub);

    std::string toString();

    //following method for testing -- check maps
    rxUInt size();

private:
    typedef __gnu_cxx::hash_map<std::string, PubSubEntry*, StringHash, StringEqual> PubSubMap;
    typedef __gnu_cxx::hash_map<std::string, PubSubMap*, StringHash, StringEqual> PubSubMapMap;
    //typedef std::map<std::string, PubSubEntry*> PubSubMap;
    //typedef std::map<std::string, PubSubMap*> PubSubMapMap;
    typedef PubSubMap::iterator PubSubMapIter;
    typedef PubSubMapMap::iterator PubSubMapMapIter;

    friend class PubSubEntry;

    bool unsubscribe(PubSubMap& map, const std::string& key, Unsubscribe* unsub);
    bool unsubscribe(PubSubMapMap& map, const std::string& topic, const std::string& subtopic, Unsubscribe* unsub);

    void publish(const std::string& topic, const std::string& subtopic, Publication* pub);
    void unpublish(const std::string& topic, const std::string& subtopic, Unpublish* unpub);

    bool isWildcard(const std::string& key);

    PubSubEntry* get(PubSubMap& map, const std::string& key);
    PubSubEntry* get(const std::string& topic, const std::string& subtopic);

    PubSubMap* getMap(const std::string& subtopic);

    void cleanup(Subscription* sub);
    void cleanup(PubSubMap& map, AndOrSubscription* sub);

    std::string toString(PubSubMap*);

    PubSubMap    mTopicMap;    // Wildcard topic subscription map
    PubSubMap    mSubtopicMap; // Wildcard subtopic subscription map
    PubSubMapMap mMap;         // Subscriptions with both topic & subtopic
};

inline bool PubSubMaps::isWildcard(const std::string& key)
{
    return key.size() == 0;
}

inline PubSubEntry* PubSubMaps::get(PubSubMap& map, const std::string& key)
{
    PubSubMapIter iter = map.find(key);

    if (iter != map.end()) {
        return iter->second; // FOUND: Return existing PubSubEntry
    }

    // NOT FOUND: allocate, store & return new PubSubEntry

    return map[key] = new PubSubEntry();
}

inline PubSubEntry* PubSubMaps::get(const std::string& topic, const std::string& subtopic)
{
    return get(*getMap(subtopic), topic);
}

typedef __gnu_cxx::hash_map<std::string, PubSubEntry*, StringHash, StringEqual> PubSubMap;

inline PubSubMap* PubSubMaps::getMap(const std::string& subtopic)
{
    PubSubMapMapIter iter = mMap.find(subtopic);

    if (iter != mMap.end()) {
        return iter->second; // FOUND: Return existing PubSubMap
    }

    // NOT FOUND: allocate, store & return new PubSubMap

    return mMap[subtopic] = new PubSubMap();
}

#endif //__PUBSUBMAPS_H__
