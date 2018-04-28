#include "PubSubEntry.h"
#include "PubSubRouter.h"
#include "Publication.h"
#include "Unpublish.h"
#include "Unsubscribe.h"

using namespace std;

PubSubEntry::PubSubEntry() 
{
    //printf("PubSubEntry[%d] Created\n", (int)this);
}

PubSubEntry::~PubSubEntry() 
{
    //printf("PubSubEntry[%d] destroyed\n", (int)this);
}

bool PubSubEntry::subscribe(Subscription* sub)
{
    if (mSubs.contains(sub)) {
        return false; // IGNORE DUPLICATE SUBSCRIPTIONS.
    }

    bool isPersistent = sub->isPersistent();
    bool published    = false;

    for (PubIter iter = mPubs.begin(); iter != mPubs.end(); iter++) {
        Publication* pub = *iter;

        if (!pub->contains(sub)) {
            sub->publish(pub);
            published = true;

            if (!isPersistent) {
                if (sub->isMatched()) {
                    sub->remove();
                    sub->removeTimeout();
                    cleanup(sub);
                }

                break; // DONE: Matched publication for oneshot subscription
                //return true; //return because of done
            }
        }
    }

    if (isPersistent || !published) {
        mSubs.add(sub);
    }

    return true;
}

bool PubSubEntry::unsubscribe(Unsubscribe* unsub)
{
    for (Subscription* sub = mSubs.next(); sub != &mSubs; sub = sub->next()) {
        if (unsub->equals(sub)) {
            sub->remove();
            sub->unsubscribe(unsub);
            delete sub;
            return isEmpty();
        }
    }

    return isEmpty();
}

bool PubSubEntry::publish(Publication* pub)
{
    for (Subscription* sub = mSubs.next(); sub != &mSubs; sub = sub->next()) {
        sub->publish(pub);

        if (sub->isMatched() && !sub->isPersistent()) {
            sub->removeTimeout();
            Subscription* prev = sub->remove();
            cleanup(sub);
            sub =  prev;
        }
    }

    if (pub->hasTTL()) {
        pub->add(mPubs);
    }

    return isEmpty();
}

bool PubSubEntry::unpublish(Unpublish* unpub)
{
    for (PubIter iter = mPubs.begin(); iter != mPubs.end(); iter++) {
        Publication* pub = *iter;

        if (pub->equals(unpub)) {
            if (pub->remove(mPubs, iter)) {
                delete pub;
            }

            return isEmpty();
        }
    }

    return isEmpty();
}

void PubSubEntry::cleanup(Subscription* sub)
{
    PubSubRouter::getRouter().mPubSubMaps.cleanup(sub);
}

string PubSubEntry::toString()
{
    string result;

    result += "SUBS: ";

    for (Subscription* sub = mSubs.next(); sub != &mSubs; sub = sub->next()) {
        result += sub->toString() + " -> ";
    }

    result += "0\nPUBS: ";

    for (PubIter iter = mPubs.begin(); iter != mPubs.end(); iter++) {
        result += (*iter)->toString() + " -> ";
    }

    result += "0\n";

    return result;
}

