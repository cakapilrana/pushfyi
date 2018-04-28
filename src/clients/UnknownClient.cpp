/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI RS SSL TCP Server Listener.

*********************************************************
*/

#include "UnknownClient.h"

UnknownClient::UnknownClient(rxUInt fd, rxUInt notificationFD,
						string ip, rxUShort port) : PushFYIClient(fd, notificationFD, ip, port)
{

}

UnknownClient::~UnknownClient()
{

}

void UnknownClient::operator=(const UnknownClient& rhs)
{
	INFO(Log::eNetwork, "TextClient Assignment Called");
}

PushFYIClient::TASK_STATUS	UnknownClient::doHandshake()
{
	Synchronized lock(mMutex);
	return FAILED;
}

PushFYIClient::TASK_STATUS	UnknownClient::read(Event& ev)
{
	Synchronized lock(mMutex);
	return FAILED;
}

PushFYIClient::TASK_STATUS	UnknownClient::write(Event& ev)
{
	Synchronized lock(mMutex);
	return FAILED;
}

PushFYIClient::TASK_STATUS UnknownClient::process()
{
	Synchronized lock(mMutex);
	return FAILED;
}

int UnknownClient::ping()
{
	return 0;
}
