#include "SSLServerSocket.h"
#include "Log.h"

SSLServerSocket::SSLServerSocket(rxInt fd, string ip, rxUShort port) : ClientSocket(fd, ip, port),
																		mSSL(0)
{
	mSSLHandshakeDone = false;
}

SSLServerSocket::~SSLServerSocket()
{
	if(mSSL!=0) {
		SSL_free(mSSL);
		mSSL = 0;
	}
}

rxInt SSLServerSocket::Accept(SSL_CTX* pSSL_CTX)
{
	mSSL = SSL_new(pSSL_CTX);
	rxInt fd = getSocket();

	SSL_set_fd(mSSL, fd);

	int ret = SSL_accept(mSSL);
	switch(SSL_get_error(mSSL, ret)) {
		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		{
			INFO(Log::eNetwork, "SSL Handshake is in progress on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			return 1;
		}
		break;
		case SSL_ERROR_SYSCALL:
		{
			INFO(Log::eNetwork, "SSL syscall error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			return -1;
		}
		break;
		case SSL_ERROR_SSL:
		{
			INFO(Log::eNetwork, "SSL library error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			return -1;
		}
		break;
		case SSL_ERROR_NONE:
		{
			INFO(Log::eNetwork, "SSL handshake on fd = %d complete", fd);
			mSSLHandshakeDone = true;
			return 0;
		}
		break;
		default:
		{
			INFO(Log::eNetwork, "SSL unknown error during handshake on fd = %d %s", fd, ERR_error_string(ERR_get_error(), NULL));
			return -1;
		}
		break;
	}
	return -1;
}

void SSLServerSocket::Close()
{
	if(mSSL!=0)
		SSL_free(mSSL);

	mSSL = 0;
	mSSLHandshakeDone = false;

	ClientSocket::Close();
}

ssize_t SSLServerSocket::read(char *buf, size_t size)
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

ssize_t SSLServerSocket::write(const char *out, size_t length)
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

