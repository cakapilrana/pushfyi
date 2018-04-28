#include "AndOrSubscription.h"
#include "PubSubMaps.h"
#include "SubTimerTask.h"

using namespace std;

AndOrSubscription::AndOrSubscription(AndOrSubscription* sub)
    : Subscription(sub), mTopics(new vector<string>(*sub->mTopics)), 
      mPrevSibling(this), mNextSibling(this)
{
    // nop
}

AndOrSubscription::AndOrSubscription(const string& topic, AndOrSubscription* sub, AndOrSubscription*& root)
    : Subscription(sub), mTopics(0), mPrevSibling(this), mNextSibling(this)
{
    mTopic = topic;

    if (root == 0) {
        root = this;
    }
    else {
        mNextSibling                     = root;
        mPrevSibling                     = root->mPrevSibling;
        root->mPrevSibling->mNextSibling = this;
        root->mPrevSibling               = this;
    }
}

AndOrSubscription::AndOrSubscription(vector<string>*  topics, 
                                     const string&    subtopic, 
                                     const string&    context, 
                                     const string&    subcontext, 
                                     Pushable<Event>* pushable, 
                                     rxInt            timeout, 
                                     const string&    mappedTopic, 
                                     rxUInt           priority)
    : Subscription(RX_ESREF, subtopic, context, subcontext, 
                   pushable, timeout, mappedTopic, priority), 
      mTopics(new vector<string>(*topics)), mPrevSibling(this), mNextSibling(this)
{
    // nop
}

AndOrSubscription::~AndOrSubscription()
{
    if (mTopics != 0) {
        delete mTopics;
    }
    for (AndOrSubscription* sub = mNextSibling; sub != this; ) {
        AndOrSubscription* next = sub->nextSibling();

        sub->remove();
        sub->removeSibling();
        delete sub;
        sub = next;
    }
}

void AndOrSubscription::process(PubSubMaps& maps)
{
    maps.subscribe(this);
}

Subscription* AndOrSubscription::remove()
{
    Subscription*      prev = mPrev;
    AndOrSubscription* sub  = this;

    do {
        sub->Subscription::remove();
        sub = sub->mNextSibling;
    } while (sub != this);

    return prev;
}

SubTimerTaskRep* AndOrSubscription::createTimeout(const timeval* expiryTime)
{
    return new SubTimerTaskRep(this, expiryTime);
}

string AndOrSubscription::toString()
{
    string             str = "S" + StringUtil::toString(getId()) + "(";
    char               op  = getOperator();
    AndOrSubscription* sub = this;

    do {
        str += sub->Subscription::toString();
        sub  = sub->mNextSibling;

        if (sub != this) {
            str += op;
        }
    } while (sub != this);

    str += ")";

    return str;
}

