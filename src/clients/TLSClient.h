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

#ifndef __PUSHFYI_TLS_CLIENT__
#define __PUSHFYI_TLS_CLIENT__

#include "PushFYIClient.h"
#include "tls.h"

class TLSClient : public PushFYIClient {
public:

	typedef enum __tls_handshake_status {
		eSSLHandshakePending = 0,
		eSSLHandshakeInProgress,
		eSSLHandshakeDone,
		eSSLHandshakeFailed,
		eSSLHandshakeTerminated
	} TLS_HANDSHAKE_STATUS;

public:
	TLSClient(rxUInt fd, rxUInt notificationFD, string ip, rxUShort port);
	virtual ~TLSClient();

	/**
	 * Overrides
	 */
	virtual TASK_STATUS	doHandshake();
	virtual TASK_STATUS read(Event&);
	virtual TASK_STATUS write(Event&);
	virtual TASK_STATUS process();
	virtual int ping();

	virtual string getSecurity() {
		return "tls";
	}

private:
	TLSClient(const TLSClient&);

	/*
	 *
	 */
	TASK_STATUS doPushFYIHandshake();

	/*
	 * TLS Handshake Status
	 */
	TLS_HANDSHAKE_STATUS	mTLSHandshakeStatus;

	/*
	 * SSL Session for this client
	 */
	SSL*	mSSL;
};

#endif //__PUSHFYI_TLS_CLIENT__
