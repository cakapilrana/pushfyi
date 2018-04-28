/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part on in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Pushfyi TCP Server Socket Wrapper

*********************************************************
*/
#ifndef __SERVER_SOCKET__
#define __SERVER_SOCKET__

#include <string>
#include "pushfyi-types.h"

class ServerSocket
{
public:
    ServerSocket();

    virtual ~ServerSocket();

    rxStatus bindAndListen();
    rxStatus bindAndListen(int port);
    rxStatus bindAndListen(std::string address, int port);

    virtual void Accept();

    virtual void processNewClientSocket(int socketFd, char* ip, rxUShort port) = 0;
    virtual void setSocketOption(int s);
    virtual void idle(int);
    virtual void close();

    void setHost(const std::string& host);
    void setPort(rxInt port);
    rxInt getPort() const;
    std::string getHost() const;

protected:
    int  			mSocket;
    std::string 	mHost;
    rxInt  			mPort;
    bool			mFinished;
};

inline void ServerSocket::setHost(const std::string& host)
{
	mHost = host;
}

inline void ServerSocket::setPort(rxInt port)
{
	mPort = port;
}

inline rxInt ServerSocket::getPort() const { return mPort; }

inline std::string ServerSocket::getHost() const { return mHost; }

#endif //__SERVER_SOCKET__
