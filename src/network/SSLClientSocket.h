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
#ifndef __SSL_CLIENT_SOCKET__
#define __SSL_CLIENT_SOCKET__

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ClientSocket.h"
#include "pushfyi-types.h"

using namespace std;

#define CIPHER_LIST "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"

class SSLClientSocket : public ClientSocket {
public:
    SSLClientSocket(rxInt fd, string ip, rxUShort port);
    virtual ~SSLClientSocket();

    virtual rxInt Connect();
    virtual void Close();

    virtual ssize_t read(char *buf, size_t size);
    virtual ssize_t write(const char *buf, size_t length);

    inline bool isSSLHandshakeDone();

private:
	/*
	 * Openssl context
	 */
	SSL_CTX* mSSLCTX;
	SSL*	 mSSL;

protected:
	bool mSSLHandshakeDone;
};

inline bool SSLClientSocket::isSSLHandshakeDone()
{
	return mSSLHandshakeDone;
}

#endif //__SSL_CLIENT_SOCKET__
