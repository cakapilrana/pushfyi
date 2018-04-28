/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET EPOLL EVENT PROXY

*********************************************************
*/
#ifndef __EPOLL_PROXY__
#define __EPOLL_PROXY__

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "pushfyi-types.h"
#include "PushFYIClient.h"


class EpollProxy {
public:
    EpollProxy(rxUInt maxEvents);
    virtual ~EpollProxy();

    /*
     * SET THE FD NON BLOCKING. THIS IS A MUST AS WE WILL
     * BE WATCHING FDs AS EDGE TRIGGERED
     */
    rxInt setnonblocking(rxUInt fd, bool mode = true);

    /*
     * ADD FD TO EPOLL SET
     */
    rxInt addEpollFd(rxUInt fd);

    /*
     * ADD FD TO EPOLL SET
     */
    rxInt addEpollFd(PushFYIClient* pClient);

    /*
     * REMOVE FD FROM EPOLL SET
     */
    rxInt removeEpollFd(rxInt fd);

    /*
     * WAIT FOR EPOLL EVENTS
     */
    rxUInt EpollWait(rxUInt timeout);

    /*
     *  READ FROM THE EPOLL FD
     */
    rxUInt EpollRead(rxUInt fd);

    inline epoll_event* getEvent(rxInt);


    /*
     *          Get Epoll Instance.
     *  This instance shall be usedd by client's
     *  for notification purpose only. Do nothing
     *  else with it.
     *
     *  This is a dangerous piece. But no other option
     */
     inline rxUInt getEpollInstance();

protected:
    /*
     *  EPOLL Instance
     */
    rxUInt  mEpollFd;

    /*
     *  EPOLL EVENTS
     */
    struct epoll_event* mEvents;

    /*
     *  MAX EPOLL EVENTS TO WATCH
     */
    rxUInt mMaxEvents;
};

inline epoll_event* EpollProxy::getEvent(rxInt index)
{
    return &mEvents[index];
}

inline rxUInt EpollProxy::getEpollInstance()
{
    return mEpollFd;
}

#endif //__EPOLL_PROXY__
