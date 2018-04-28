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

#ifndef __PUSHFYI_TEXT_CLIENT__
#define __PUSHFYI_TEXT_CLIENT__

#include "PushFYIClient.h"

class TextClient : public PushFYIClient {

public:
	TextClient(rxUInt fd, rxUInt notificationFD, string ip, rxUShort port);
	virtual ~TextClient();
	void operator=(const TextClient& rhs);

	/**
	 * Overrides
	 */
	virtual TASK_STATUS	doHandshake();
	virtual TASK_STATUS read(Event&);
	virtual TASK_STATUS write(Event&);
	virtual TASK_STATUS process();

	virtual int ping();

	virtual string getSecurity() {
		return "text";
	}
private:
	TextClient(const TextClient&);

};

#endif //__PUSHFYI_TEXT_CLIENT__
