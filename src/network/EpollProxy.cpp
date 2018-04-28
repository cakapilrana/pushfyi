/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part on in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET EPOLL EVENT PROXY

*********************************************************
*/
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "EpollProxy.h"
#include "Log.h"

EpollProxy::EpollProxy(rxUInt maxEvents) : mEpollFd(0), mMaxEvents(maxEvents)
{
    mEpollFd = epoll_create(mMaxEvents);

    mEvents = (epoll_event*) calloc(mMaxEvents, sizeof(struct epoll_event));
}

EpollProxy::~EpollProxy()
{
}

rxInt EpollProxy::setnonblocking(rxUInt fd, bool mode)
{
    int flags, s;

    flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1) {
        ERROR(Log::eNetwork, "Error setting socket nonblocking, fd = %d", fd);
        return -1;
    }

    if(mode)
    	flags |= O_NONBLOCK;
    else
    	flags &= ~O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if(s == -1) {
        ERROR(Log::eNetwork, "Error setting socket nonblocking, fd = %d", fd);
        return -1;
    }

    if(mode)
    	INFO(Log::eNetwork, "fd = %d is set non blocking", fd);
    else
    	INFO(Log::eNetwork, "fd = %d is set blocking", fd);

    return 0;
}

rxInt EpollProxy::addEpollFd(rxUInt fd)
{
    rxInt ret = 0;
    struct epoll_event event;
    //First off, set the FD non blocking
    ret = setnonblocking(fd);
    if(ret == 0) {
        /*
         * TODO:
         * For better throughput we may think of working in EPOLLET
         *
         * For now we are using level triggered for sake of ease.
         */
        event.data.fd = fd;
        event.events = EPOLLIN;

        ret = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event);
        if(ret == -1) {
            ERROR(Log::eNetwork, "Error adding fd to epoll set, fd = %d", fd);
            return -1;
        }

        INFO(Log::eNetwork, "Epoll FD Added = %d", fd);
    }

    return ret;
}


rxInt EpollProxy::addEpollFd(PushFYIClient* pClient)
{
    rxUInt fd = pClient->getSocket();
    rxInt ret = 0;
    struct epoll_event event;
    //First off, set the FD non blocking
    ret = setnonblocking(fd);
    if(ret == 0) {
        /*
         * TODO:
         * For better throughput we may think of working in EPOLLET
         *
         * For now we are using level triggered for sake of ease.
         */
        event.data.ptr = pClient;
        event.events = EPOLLIN;

        ret = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &event);
        if(ret == -1) {
            ERROR(Log::eNetwork, "Error adding fd to epoll set, fd = %d", fd);
            return -1;
        }

        INFO(Log::eNetwork, "Epoll FD Added = %d", fd);
    }

    return ret;
}

rxInt EpollProxy::removeEpollFd(rxInt fd)
{
    rxInt ret = 0;

    //TODO
    ret = setnonblocking(fd, false);

    ret = epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL);
    if(ret == -1) {
        ERROR(Log::eNetwork, "Error removing fd from epoll set, fd = %d", fd);
        return -1;
    }

    INFO(Log::eNetwork, "Epoll FD Removed = %d", fd);
    return ret;
}

rxUInt EpollProxy::EpollWait(rxUInt timeout)
{
    return epoll_wait(mEpollFd, mEvents, mMaxEvents, timeout);
}

rxUInt EpollProxy::EpollRead(rxUInt fd)
{
    bool closed = false;
    ssize_t count = 0;
    char buf[2048] = {0};

    while (true) {
        count = read(fd, buf, sizeof(buf));
        if( count == -1 ) {
            //error during read
            if( errno != EAGAIN || errno != EWOULDBLOCK ) {
                //we have read all the data.
            }
            break;
        }

        else if( count == 0 ) {
            //client closed his connection
            closed = true;
            removeEpollFd(fd);
            break;
        }

        //read more bytes
    }
    if(!closed) {
        INFO(Log::eNetwork, "Data read from fd = %d, data = %s", fd, buf);
        char res[2048] = {0};
        strcpy(res, "Congratulations your message is received = ");
        strcat(res, buf);
        write(fd, res, strlen(res));
    }
    else
        INFO(Log::eNetwork, "Connection closed by client = %d", fd);
}

