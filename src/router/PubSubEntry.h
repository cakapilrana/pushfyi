#ifndef __PUBSUBENTRY_H__
#define __PUBSUBENTRY_H__

#include "Subscription.h"
#include "Publication.h"

class Unsubscribe;
class Unpublish;

class PubSubEntry
{
public:
    PubSubEntry();
    ~PubSubEntry();

    bool subscribe(Subscription* sub);
    bool unsubscribe(Unsubscribe* unsub);

    bool publish(Publication* pub);
    bool unpublish(Unpublish* unpub);

    bool isEmpty();

    std::string toString();

private:
    void cleanup(Subscription* sub);

    Publications mPubs;
    Subscription mSubs;
};

inline bool PubSubEntry::isEmpty()
{
    return mPubs.empty() && (mSubs.next() == &mSubs);
}

#endif //__PUBSUBENTRY_H__
