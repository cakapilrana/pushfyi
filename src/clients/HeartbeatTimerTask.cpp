#include "HeartbeatTimerTask.h"
#include "PushFYIClient.h"

using namespace std;

HeartbeatTimerTaskRep::HeartbeatTimerTaskRep() : mClient(0)
{
    // nop
}

HeartbeatTimerTaskRep::HeartbeatTimerTaskRep(PushFYIClient* client, const timeval* expiryTime)
    : TimerCbRep(expiryTime), mClient(client)
{
    // nop
}

HeartbeatTimerTaskRep::HeartbeatTimerTaskRep(HeartbeatTimerTaskRep& toCopy)
    : TimerCbRep(toCopy), mClient(toCopy.mClient)
{
    // nop
}

HeartbeatTimerTaskRep::~HeartbeatTimerTaskRep()
{
    // nop
}

void HeartbeatTimerTaskRep::process(timeval* /* tv */)
{
	if(mClient) {
		int ret = mClient->ping();
	}
}

HeartbeatTimerTask::HeartbeatTimerTask()
{
    // nop
}

HeartbeatTimerTask::HeartbeatTimerTask(HeartbeatTimerTaskRep* rep)
    : TimerCb(rep)
{
    // nop
}

HeartbeatTimerTask::HeartbeatTimerTask(const HeartbeatTimerTask& toCopy)
    : TimerCb(toCopy)
{
    // nop
}

HeartbeatTimerTask::~HeartbeatTimerTask()
{
    // nop
}

