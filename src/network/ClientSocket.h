/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: TCP/IP Client Socket Wrapper

*********************************************************
*/
#ifndef __CLIENT_SOCKET__
#define __CLIENT_SOCKET__

#include <unistd.h>
#include <string>
#include "pushfyi-types.h"

using namespace std;

class ClientSocket {
public:
    ClientSocket(rxInt fd, string ip, rxUShort port);
    virtual ~ClientSocket();

    virtual rxInt Connect();
    virtual void Close();

    //virtual ssize_t read(char *buf, size_t size) = 0;
    //virtual ssize_t write(const char *buf, size_t length) = 0;

    inline  rxInt getSocket() { return mSocket; }
    inline  string getIP() { return mIP; }
    inline  rxUShort getPort() { return mPort; }

    inline void setIP(string ip) { mIP = ip; }
    inline void setPort(rxUShort port) { mPort = port; }

    inline bool isConnected();

protected:
    /*
     *  CLIENT SOCKET
     */
    rxInt mSocket;

    string mIP;

    rxUShort mPort;

    bool mConnected;

};

inline bool ClientSocket::isConnected() {
	return mConnected;
}

#endif //__CLIENT_SICKET__
