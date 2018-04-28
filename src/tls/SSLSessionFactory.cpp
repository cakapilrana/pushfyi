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

#include "SSLSessionFactory.h"
#include "Properties.h"
#include "ProcessInit.h"
#include "Log.h"

#define CIPHER_LIST "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH"

SSLSessionFactory* SSLSessionFactory::mInstance = 0;

SSLSessionFactory* SSLSessionFactory::getInstance()
{
	if(mInstance == 0) {
		mInstance = new SSLSessionFactory();
	}

	return mInstance;
}

SSLSessionFactory::SSLSessionFactory() : mSSLCTX(0)
{
	/**
	 * Initialize the SSL CONTEXT
	 *
	 * This is the SSL Template Context that will be be used
	 * in creating SSL Sessions for clients
	 */
	loadCertificates();

	/*
	 * We will support both SSL and TLS
	 * However we will remove support for SSL v2.
	 */
	const SSL_METHOD* pSSLMethod = SSLv23_server_method();

    char err[1024] = {0};
    mSSLCTX = SSL_CTX_new(pSSLMethod);
    if(mSSLCTX == NULL) {
        ERROR(Log::eNetwork, "Failed to initialize SLS_CTX. TLS Unavaliable.");
        ERROR(Log::eNetwork, ERR_error_string(ERR_get_error(), err));
    } else
        INFO(Log::eNetwork, "Initialized SSL Context.");

    //we do not want the client certificate.
    SSL_CTX_set_verify(mSSLCTX, SSL_VERIFY_NONE, 0);
    SSL_CTX_set_options(mSSLCTX, SSL_OP_ALL | SSL_OP_NO_SSLv2);

    if(SSL_CTX_set_cipher_list(mSSLCTX, CIPHER_LIST) != 1) {
    	ERROR(Log::eNetwork, "SSL_CTX_set_cipher_list cipher list invalid");
    }

    //Load the SSL Certificates
    rxUInt ret = 0;
    ret = SSL_CTX_use_certificate_file(mSSLCTX, mCACertificateFile.c_str(), SSL_FILETYPE_PEM);
    if(ret <=0 ) {
        ERROR(Log::eNetwork, "Failed to read ssl certificate at %s. We should exit.", mCACertificateFile.c_str());
        ERROR(Log::eNetwork, ERR_error_string(ERR_get_error(), err));
    } else
        INFO(Log::eNetwork, "SSL Certificate read complete. OK!");

    //Load the private key file
    ret = SSL_CTX_use_PrivateKey_file(mSSLCTX, mPrivateKeyFile.c_str(), SSL_FILETYPE_PEM);
    if(ret <=0 ) {
        ERROR(Log::eNetwork, "Failed to read private key file at %s. We should exit.", mPrivateKeyFile.c_str());
        ERROR(Log::eNetwork, ERR_error_string(ERR_get_error(), err));
    } else
        INFO(Log::eNetwork, "Private Key read complete. OK!");

    //Load certificate chain
    ret = SSL_CTX_load_verify_locations(mSSLCTX, mCAChainFile.c_str(), mCAChainFileLocation.c_str());
    if(ret <=0 ) {
        ERROR(Log::eNetwork, "Failed to CA chain file at %s. We should exit.", mCAChainFile.c_str());
        ERROR(Log::eNetwork, ERR_error_string(ERR_get_error(), err));
    } else
        INFO(Log::eNetwork, "CA Chain File read complete. OK!");

    if(!SSL_CTX_check_private_key(mSSLCTX)) {
        ERROR(Log::eNetwork, "Private key does not match the SSL Certificate. We should exit.");
        ERROR(Log::eNetwork, ERR_error_string(ERR_get_error(), err));
    } else
        INFO(Log::eNetwork, "Private Key and SSL Certificate Matched. OK!");

    /*
     * Set the SSL_CTX Options
     */
    SSL_CTX_set_read_ahead(mSSLCTX, 0);
}

SSLSessionFactory::~SSLSessionFactory()
{

}

SSL* SSLSessionFactory::getClientSSLSession(rxUInt fd)
{
	Synchronized lock(mMutex);

	if(mSSLCTX) {
		SSL* pSSL = SSL_new(mSSLCTX);
		SSL_set_fd(pSSL, fd);

		return pSSL;
	}

	return 0;
}

void SSLSessionFactory::loadCertificates()
{
	Properties* sysProps = ProcessInit::instance()->getSysProps();

	string certificate, privatekey, chainfile, location;

    /*
     *  Set CA Certificate File
     */
    if(sysProps->contains("pushfyirs.secure.tcp.server.certificate") &&
    		sysProps->contains("keyFilePath"))
    	mCACertificateFile = sysProps->get("keyFilePath") +
    						sysProps->get("pushfyirs.secure.tcp.server.certificate");
    else
    	/*
    	 * During development use a self signed certificate file.
    	 *
    	 * A private key and self signed certificate can be generated
    	 * using the following openssl command.
    	 *
    	 * openssl req -x509 -sha256 -nodes -days 365 -newkey rsa:2048
    	 * 		-keyout private.pem
    	 * 		-out self.crt
    	 */
    	mCACertificateFile = sysProps->get("keyFilePath") + "pushfyi.crt";


    /*
     *  Set Private Key File
     */
    if(sysProps->contains("pushfyirs.secure.tcp.server.privatekey") &&
    		sysProps->contains("keyFilePath"))
    	mPrivateKeyFile = sysProps->get("keyFilePath") +
    						sysProps->get("pushfyirs.secure.tcp.server.privatekey");
    else
    	mPrivateKeyFile = sysProps->get("keyFilePath") + "pushfyi.key";

    /*
     *  Set CA Chain File
     */
    if(sysProps->contains("pushfyirs.secure.tcp.server.cafile") &&
    		sysProps->contains("keyFilePath"))
    	mCAChainFile = sysProps->get("keyFilePath") +
    						sysProps->get("pushfyirs.secure.tcp.server.cafile");
    else
    	mCAChainFile = sysProps->get("keyFilePath") + "pushfyi.key";

    /*
     * Set chain file location
     */
    mCAChainFileLocation = sysProps->get("keyFilePath");
}

/*
rxInt SSLContextFactory::NewSSLSocket(SSL** pSSL, rxUInt fd)
{
	//usleep(500);
	*pSSL = SSL_new(mSSLContext);
	SSL_set_fd(*pSSL, fd);

	int ret = SSL_accept(*pSSL);
	switch(SSL_get_error(*pSSL, ret)) {
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		{
			INFO(Log::eNetwork, "SSL Handshake is in progress on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			SSL_free(*pSSL);
			return SSL_ERROR_WANT_READ;
		}
		break;
		case SSL_ERROR_SYSCALL:
		{
			INFO(Log::eNetwork, "SSL syscall error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			SSL_free(*pSSL);
			return SSL_ERROR_SYSCALL;
		}
		break;
		case SSL_ERROR_SSL:
		{
			INFO(Log::eNetwork, "SSL library error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			SSL_free(*pSSL);
			return SSL_ERROR_SSL;
		}
		break;
		case SSL_ERROR_NONE:
		{
			INFO(Log::eNetwork, "SSL handshake on fd = %d complete", fd);
			return SSL_ERROR_NONE;
		}
		break;
		default:
		{
			INFO(Log::eNetwork, "SSL unknown error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			SSL_free(*pSSL);
			return -1;
		}
		break;
	}
	return -1;
}

SSL* SSLContextFactory::GetSSLClientSocket(rxUInt fd)
{
	SSL* pSSL = SSL_new(mSSLContext);
	SSL_set_fd(pSSL, fd);

	return pSSL;
}
*/
