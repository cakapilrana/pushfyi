/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: PUSH FYI OPEN SSL WRAPPER

*********************************************************
*/

#ifndef __SSL_SESSION_FACTORY__
#define __SSL_SESSION_FACTORY__

#include <string>
#include "tls.h"
#include "pushfyi-types.h"
#include "Mutex.h"

using namespace std;

class SSLSessionFactory {
private:
	SSLSessionFactory();
	SSLSessionFactory(const SSLSessionFactory&);

	static SSLSessionFactory* mInstance;

	void loadCertificates();

public:
	virtual ~SSLSessionFactory();
	static SSLSessionFactory* getInstance();

    SSL* getClientSSLSession(rxUInt fd);

private:

    string      mCACertificateFile;
    string      mPrivateKeyFile;
    string		mCAChainFile;
    string 		mCAChainFileLocation;

private:
    /**
     * SSL Context
     */
    SSL_CTX*	mSSLCTX;

    /*
     * Mutex
     */
    Mutex		mMutex;
};

#endif //__SSL_CONTEXT_FACTORY__
