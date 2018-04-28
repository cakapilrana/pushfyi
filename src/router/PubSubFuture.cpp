#include "PubSubFuture.h"
#include "PubSubRouter.h"

using namespace std;

rxUInt PubSubFuture::UNIQUE_ID = 0;
Mutex  PubSubFuture::ID_MUTEX;

PubSubFuture::PubSubFuture(const std::string& topic, 
                           const std::string& subtopic, 
                           rxUInt             timeout, 
                           rxUInt             priority)
    : Pushable<Event>(), mEvent(0)
{
    PubSubRouter&  pubSub  = PubSubRouter::getRouter();
    string         context = createUniqueId();

    pubSub.subscribe(context, "_PubSubFuture", this, topic, subtopic, timeout, RX_ESREF, priority);
}

PubSubFuture::~PubSubFuture()
{
    // nop
}

void PubSubFuture::push(Event event)
{
    Synchronized lock(mMutex);

    mEvent = event;
    mNotPublished.notifyAll();
}

void PubSubFuture::pushFront(Event event)
{
    Synchronized lock(mMutex);

    mEvent = event;
    mNotPublished.notifyAll();
}

Event PubSubFuture::getEvent()
{
    Synchronized lock(mMutex);

    while (mEvent.isNull()) {
        mNotPublished.wait(lock);
    }

    return mEvent;
}

