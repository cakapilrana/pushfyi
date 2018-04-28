#include "AndSubscription.h"
#include "PubSubMaps.h"
#include "SubTimerTask.h"

using namespace std;

AndSubscription::AndSubscription(AndSubscription* sub)
    : AndOrSubscription(sub), mEvent(sub->mEvent), mAllMatched(sub->mAllMatched)
{
    // nop
}

AndSubscription::AndSubscription(const string& topic, AndOrSubscription* sub, AndOrSubscription*& root)
    : AndOrSubscription(topic, sub, root), mEvent(0), mAllMatched(false)
{
    // nop
}

AndSubscription::AndSubscription(vector<string>*  topics, 
                                 const string&    subtopic, 
                                 const string&    context, 
                                 const string&    subcontext, 
                                 Pushable<Event>* pushable, 
                                 rxInt            timeout, 
                                 const string&    mappedTopic, 
                                 rxUInt           priority)
    : AndOrSubscription(topics, subtopic, context, subcontext, 
                        pushable, timeout, mappedTopic, priority), 
      mEvent(0), mAllMatched(false)
{
    // nop
}

AndSubscription::~AndSubscription()
{
    // nop
}

void AndSubscription::publish(Publication* pub)
{
    Event event = pub->getEvent();

    mIsMatched = true;

    // If the mapped topic equals the published Event then store this
    // Event for when this subscription completely matches. This will be
    // the event published to the subscriber.
    if (mMappedTopic.size() > 0 && mMappedTopic == event->getTopic()) {
        setEvent(event);
    }

    if (!allMatched()) {
        if (isPersistent()) {
            pub->addSub(new Subscription(this));
        }
        else {
            pub->addSub(this);
            removeSibling();
            Subscription::remove();
        }

        return; // RETURN: Not fully matched yet
    }

    // The root subscription has an event to publish (i.e. a previously
    // published event had a topic which matched the mapped topic.
    if (!mEvent.isNull()) {
        pub->setEvent(mEvent);
    }

    if (isPersistent()) {
        pub->publish(new Subscription(this));
        reset();
    }
    else {
        pub->publish(this);
    }
}

void AndSubscription::unsubscribe(Unsubscribe* unsub)
{
    Subscription::unsubscribe(unsub);

    for (AndOrSubscription* sub = nextSibling(); sub != this; ) {
        AndOrSubscription* next = sub->nextSibling();

        sub->removeSibling();
        sub->Subscription::remove();

        if (!sub->Subscription::isMatched()) {
            delete sub;
        }

        sub = next;
    }
}

bool AndSubscription::allMatched()
{
    if (mAllMatched) {
        return mAllMatched;
    }

    AndSubscription* sub = this;

    do {
        if (!sub->Subscription::isMatched()) {
            return false; // NOT ALL MATCHED
        }
        sub = (AndSubscription*) sub->nextSibling();
    } while (sub != this);

    // Set mAllMatched for each sibling
    do {
        sub->mAllMatched = true;
        sub = (AndSubscription*) sub->nextSibling();
    } while (sub != this);

    return true; // ALL MATCHED
}

void AndSubscription::reset()
{
    AndSubscription* sub = this;

    do {
        sub->mAllMatched = false;
        sub->Subscription::reset();
        sub = (AndSubscription*) sub->mNextSibling;
    } while (sub != this);
}

