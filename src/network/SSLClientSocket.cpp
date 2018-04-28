#include "SSLClientSocket.h"
#include "Log.h"

SSLClientSocket::SSLClientSocket(rxInt fd, string ip, rxUShort port) : ClientSocket(fd, ip, port),
																		mSSLCTX(0), mSSL(0)
{
	const SSL_METHOD* pSSLMethod = SSLv23_client_method();
	mSSLCTX = SSL_CTX_new(pSSLMethod);

    SSL_CTX_set_options(mSSLCTX, SSL_OP_ALL | SSL_OP_NO_SSLv2);

    if(SSL_CTX_set_cipher_list(mSSLCTX, CIPHER_LIST) != 1)
    {
    	//log error
    }
    mSSLHandshakeDone = false;
}

SSLClientSocket::~SSLClientSocket()
{
	if(mSSLCTX)
		SSL_CTX_free(mSSLCTX);
}

rxInt SSLClientSocket::Connect()
{
	if(mSSLCTX != 0) {

		if(mSSL == 0) {
			INFO(Log::eNetwork, "Creating SSL Context");
			mSSL = SSL_new(mSSLCTX);
			SSL_set_fd(mSSL, mSocket);
		}

		else {
			INFO(Log::eNetwork, "Sending ssl client hello");
			int ret = SSL_connect(mSSL);
			switch(SSL_get_error(mSSL, ret)) {
				case SSL_ERROR_NONE:
				{
					//Handshake complete!!!
					INFO(Log::eNetwork, "SSL Handshake with Pushfyi complete");
					mSSLHandshakeDone = true;

					return 0;
				}
				break;

				case SSL_ERROR_ZERO_RETURN:
				{
					INFO(Log::eNetwork, "Server closed the connection during handshake");
					return -1;
				}
				break;

				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_READ:
				{
					INFO(Log::eNetwork, "SSL Handshake is in progress");
					return 1;
				}
				break;

				case SSL_ERROR_SSL:
				{
					/*
					 * TODO:
					 * This is some shity error. Just terminate for now.
					 * Allow the library to reconnect.
					 *
					 * Sooner or later we must handle it gracefully
					 */
					INFO(Log::eNetwork, "SSL Library Error. Handshake terminated.");
					return -1;
				}
				break;

				/*Any other error, just terminate. We are not in a position to handle*/
				default:
				{
					/*
					 * Some unknown error from the openssl. This should'nt be happening.
					 * However, now it has happened. Just terminate the connection and
					 * allow the library to reconnect.
					 *
					 * Sooner or later we must handle it gracefully
					 */
					INFO(Log::eNetwork, "SSL Unknown Error. Handshake terminated.");
					return -1;
				}
			}
		}

	}
	return 0;
}

void SSLClientSocket::Close()
{
	if(mSSL!=0)
		SSL_free(mSSL);

	mSSL = 0;
	mSSLHandshakeDone = false;

	ClientSocket::Close();
}

ssize_t SSLClientSocket::read(char *buf, size_t size)
{
	int ret = 0;
	ret = SSL_read(mSSL, buf, size);
	switch(SSL_get_error(mSSL, ret))
	{
		/*
		 * Read complete :)
		 */
		case SSL_ERROR_NONE:
		{
			return 0;
		}
		break;

		/*
		 * Server closed the connection
		 */
		case SSL_ERROR_ZERO_RETURN:
		{
			return -1;
		}
		break;

		/*
		 * Underlying socket could'nt be read
		 * as the openssl may be still be doing its job.
		 *
		 * This is a non blocking socket!!
		 * Lets try and read in the next select cycle.
		 */
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
		{
			return 1;
		}
		break;

		/*
		 * TODO:
		 * This is some shity error. We lost a message!!!
		 *
		 * Just terminate for now.
		 *
		 * Allow the library to reconnect.
		 *
		 * Sooner or later we must handle it gracefully
		 */
		case SSL_ERROR_SSL:
		{
			return -1;
		}
		break;

		/*Any other error just terminate. We are not in a position to handle*/
		default:
		{
			/*
			 * Some unknown error from the openssl. This should'nt be happening.
			 * However, now it has happened. Just terminate the connection and
			 * allow the library to reconnect.
			 *
			 * Sooner or later we must handle it gracefully
			 */
			return -1;
		}
		break;
	}

	return ret;
}

ssize_t SSLClientSocket::write(const char *out, size_t length)
{
	int ret = 0;
	ret = SSL_write(mSSL, out, length);
	switch(SSL_get_error(mSSL, ret))
	{
		case SSL_ERROR_NONE:
		{
			return 0;
		}
		break;

		case SSL_ERROR_ZERO_RETURN:
		{
			return -1;
		}
		break;

		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_READ:
		{
			return 1;
		}
		break;

		case SSL_ERROR_SSL:
		{
			return -1;
		}
		break;

		default:
		{
			return -1;
		}
		break;
	}

	return ret;
}

