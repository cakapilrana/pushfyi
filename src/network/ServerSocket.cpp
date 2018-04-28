/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part on in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUBNET TCP Server Socket Wrapper

*********************************************************
*/
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerSocket.h"
#include "Log.h"

using namespace std;

ServerSocket::ServerSocket()
    : mSocket(-1), mHost("0.0.0.0"), mPort(0), mFinished(false)
{
}

ServerSocket::~ServerSocket()
{
}

rxStatus ServerSocket::bindAndListen(int port)
{
	mPort = port;
	return bindAndListen();
}

rxStatus ServerSocket::bindAndListen(std::string address, int port)
{
	mHost = address;
	mPort = port;
	return bindAndListen();
}

rxStatus ServerSocket::bindAndListen()
{
	int status = 0;

	mSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(mSocket < 0) {
		ERROR(Log::eNetwork, "Error in creating server socket");
		return mSocket;
	}

	setSocketOption(mSocket);

	INFO(Log::eNetwork, "Server socket created address = %s, port = %d socket fd = %d", mHost.c_str(), mPort, mSocket);

	struct sockaddr_in tcpAddress;

	memset(&tcpAddress, 0, sizeof(tcpAddress));

	unsigned long inetAddress = inet_addr(mHost.c_str());

	if(inetAddress != INADDR_NONE) {
		//its a dotted decimal address
		memcpy(&tcpAddress.sin_addr, &inetAddress, sizeof(inetAddress));
		INFO(Log::eNetwork, "Dotted decimal address encountered");
	} else {
		res_init();

		struct hostent* hostEntry = gethostbyname(mHost.c_str());

		if(hostEntry == NULL) {
			ERROR(Log::eNetwork, "gethostbyname error host = %s", mHost.c_str());
			return h_errno;
		}

		INFO(Log::eNetwork, "Host name found host = %s", mHost.c_str());
		memcpy(&tcpAddress.sin_addr, hostEntry->h_addr, hostEntry->h_length);
	}

	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_port = htons(mPort);
	status = bind(mSocket, (struct sockaddr*) &tcpAddress, sizeof(tcpAddress));

	if(status < 0) {
		ERROR(Log::eNetwork, "Bind error on port = %d", mPort);
		return status;
	}

	INFO(Log::eNetwork, "Server Socket bind OK!! host = %s, port = %d", mHost.c_str(), mPort);

	status = listen(mSocket, 1024);
	if(status < 0) {
		ERROR(Log::eNetwork, "Listen error on port = %d", mPort);
		return status;
	}

	return 0;
}

void ServerSocket::Accept()
{
	struct timeval tv_delta;
	tv_delta.tv_usec = 0;

	fd_set fdSet;
	int retval=0;
	FD_ZERO(&fdSet);

	while(!mFinished) {

		FD_SET(mSocket, &fdSet);
		tv_delta.tv_usec = 0;
		tv_delta.tv_sec = 2;

		retval = select(mSocket+1, &fdSet, NULL, NULL, &tv_delta);
		if(retval <=0) {
			if(retval==0) {
				idle(retval);
			} else {
				ERROR(Log::eNetwork, "Select failed on port = %d, error = %s", mPort, strerror(errno));
			}
			continue;
		}

		struct sockaddr peerAddress;
		socklen_t socklen = sizeof(peerAddress);

		int newSocket = accept(mSocket, &peerAddress, &socklen);
		if(newSocket == -1) {
			ERROR(Log::eNetwork, "Error in accept, listen address = %s, port = %d select returned = %d", mHost.c_str(), mPort, retval);
			continue;
		}

		rxUShort port = ntohs(*((rxUShort*) &(peerAddress.sa_data[0])));

		char ip[32] = {0};

		sprintf(ip, "%hhu.%hhu.%hhu.%hhu", peerAddress.sa_data[2],peerAddress.sa_data[3],peerAddress.sa_data[4],
				peerAddress.sa_data[5]);

		processNewClientSocket(newSocket, ip, port);
	}
	::close(mSocket);
	mSocket = 0;
}

void ServerSocket::setSocketOption(int socket)
{
	int reuseAddr = 1;

	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
}

void ServerSocket::close()
{
	::close(mSocket);
	mSocket = -1;
	//mFinished = true;
}

void ServerSocket::idle(int retval)
{
	INFO(Log::eNetwork, "Select timed out retval = %d", retval);
}
