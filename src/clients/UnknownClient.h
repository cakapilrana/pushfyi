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

#ifndef __PUSHFYI_UNKNOWN_CLIENT__
#define __PUSHFYI_UNKNOWN_CLIENT__

#include "PushFYIClient.h"

class UnknownClient : public PushFYIClient {

public:
	UnknownClient(rxUInt fd, rxUInt notificationFD, string ip, rxUShort port);
	virtual ~UnknownClient();
	void operator=(const UnknownClient& rhs);

	/**
	 * Overrides
	 */
	virtual TASK_STATUS	doHandshake();
	virtual TASK_STATUS read(Event&);
	virtual TASK_STATUS write(Event&);
	virtual TASK_STATUS process();

	virtual int ping();

	virtual string getSecurity() {
		return "unknown";
	}

private:
	UnknownClient(const UnknownClient&);
};

#endif //__PUSHFYI_UNKNOWN_CLIENT__
