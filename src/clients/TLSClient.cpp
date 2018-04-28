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

#include <sys/types.h>
#include <sys/socket.h>
#include "TLSClient.h"
#include "PushFYIProtocolFactory.h"
#include "SSLSessionFactory.h"
#include "Event.h"
#include "EventList.h"
#include "Pushfyi-Defs.h"

TLSClient::TLSClient(rxUInt fd, rxUInt notificationFD,
		string ip, rxUShort port) : PushFYIClient(fd, notificationFD, ip, port), mSSL(0)
{
	mTLSHandshakeStatus = eSSLHandshakePending;

	/*
	 * Empty the buffers
	 */
	memset(mReadPending, 0, sizeof(mReadPending));
	mReadPendingOffset = 0;

	memset(mDecodePending, 0, sizeof(mDecodePending));
	mDecodePendingOffset = 0;

	memset(mContinuationData, 0, sizeof(mContinuationData));
	mContinuationDataOffset = 0;
}

TLSClient::~TLSClient()
{
	if(mSSL) {
		SSL_free(mSSL);
		mSSL = 0;
	}
}

PushFYIClient::TASK_STATUS	TLSClient::doHandshake()
{
	Synchronized lock(mMutex);

	rxUInt fd = getSocket();

	bool isHTTPRequest = false;

	char request[4096] = {0};
	rxInt length = 0;

    /*
     * This is a TLS Client.
     * Perform the SSL Handshake
     */
	if(mTLSHandshakeStatus == eSSLHandshakePending) {

		mSecurity = ePushfyiUnknownSecurity;

		length = recv(fd, request, 4096 , MSG_PEEK);

		INFO(Log::eNetwork, "recv peeked %d bytes of data", length);
	    if(length == 0) {
			INFO(Log::eProtocol, "Ignoring empty handshake on fd = %d", fd);
			mProtocol =  eProtocolUnknown;
			return FAILED;
	    }
	    else {
	    	mSecurity = ProtocolHelper::DetectSecurityScheme(request);

			/*
			 * Discard Anything Other Than TLS 3.1
			 */
			if(mSecurity == ePushfyiPlainText ||
					mSecurity == ePushfyiUnsupportedSecurity) {
				INFO(Log::eNetwork, "unsupported security scheme for connection on fd = %d", fd);
				return FAILED;
			}
	    }
	}


    if( (mTLSHandshakeStatus == eSSLHandshakePending) &&
    		(mSecurity == ePushfyiTLS) ) {
    	/*
    	 * We have a TLS Client Trying to connect to us
    	 */
    	INFO(Log::eNetwork, "processing secure connection on fd = %d", fd);
    	/*
    	 * Initiate SSL Handshake. This must be ASYNC as our socket is
    	 * non blocking.
    	 */

    	/*
    	 * Get an instance to SSL Session factory
    	 */
    	SSLSessionFactory* sslFactory = SSLSessionFactory::getInstance();

    	/*
    	 * Get SSL session for this connection.
    	 *
    	 * This might also be a pending handshake as we are ASYNC!!!
    	 */
    	if(0 == mSSL) {
    		/*
    		 * Handshake has not started yet.
    		 */
    		INFO(Log::eNetwork, "Creating SSL session for fd = %d", fd);

    		mSSL = sslFactory->getClientSSLSession(fd);

    		if(0 == mSSL) {
    			/*
    			 * Failed to create SSL CLient Context
    			 * We can not proceed further
    			 */
    			return FAILED;
    		} else {
				/*
				 * Update the handshake in progress state
				 */
    			mTLSHandshakeStatus = eSSLHandshakeInProgress;
    		}
    	} else {
    		/*
    		 * This is an ongoing handshake that could not complete previously.
    		 */
    		INFO(Log::eNetwork, "Resuming SSL handshake with client on fd = %d", fd);
    	}
    }

    if( (mTLSHandshakeStatus == eSSLHandshakeInProgress) &&
    		(mSecurity == ePushfyiTLS) ) {
    	/*
    	 * Lets SSL Handshake
    	 */
		INFO(Log::eNetwork, "calling SSL_accept for fd = %d", fd);

		/*
		 * We have the context. respond to ClientHello
		 */
		rxUInt ret = SSL_accept(mSSL);
		if(ret > 0) {
			INFO(Log::eNetwork, "SSL_accept, handshake complete for client {%s:%d} on fd = %d",
								getIP().c_str(), getPort(), fd);
			mTLSHandshakeStatus = eSSLHandshakeDone;
		} else if(ret < 0 || ret == 0) {
			switch(SSL_get_error(mSSL, ret)) {
				case SSL_ERROR_NONE:
				{
					//Handshake complete!!!
					INFO(Log::eNetwork, "SSL_accept, handshake complete for client {%s:%d} on fd = %d",
										getIP().c_str(), getPort(), fd);
					mTLSHandshakeStatus = eSSLHandshakeDone;
				}
				break;

				case SSL_ERROR_ZERO_RETURN:
				{
					//client terminated handshake!!!
					ERROR(Log::eNetwork, "SSL_accept, handshake is terminated by client on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeTerminated;
				}
				break;

				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_READ:
				{
					INFO(Log::eNetwork, "SSL_accept, Handshake is in progress on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeInProgress;
				}
				break;

				case SSL_ERROR_SSL:
				{
					ERROR(Log::eNetwork, "SSL_accept, Critical Error during SSL Handshake on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeFailed;

					/*
					 * Lets check errno
					 */
					ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
				}
				break;

				case SSL_ERROR_WANT_ACCEPT:
				case SSL_ERROR_WANT_CONNECT:
				{
					ERROR(Log::eNetwork, "SSL_accept, Want connect Error during SSL Handshake on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeFailed;

					/*
					 * Lets check errno
					 */
					ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
				}
				break;

				case SSL_ERROR_SYSCALL:
				{
					ERROR(Log::eNetwork, "SSL_accept, SYSCALL Error during SSL Handshake on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeFailed;

					/*
					 * Lets check errno
					 */
					ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
				}
				break;

				/*Any other error just terminate. We are not in a position to handle*/
				default:
				{
					ERROR(Log::eNetwork, "SSL_accept, Unknown Error during SSL Handshake on fd = %d", fd);
					mTLSHandshakeStatus = eSSLHandshakeFailed;

					/*
					 * Lets check errno
					 */
					ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
				}
			}
		}
    }

	/*
	 * Check the status of non blocking ssl io operation
	 */
	if( (mTLSHandshakeStatus == eSSLHandshakeTerminated) ||
			(mTLSHandshakeStatus == eSSLHandshakeFailed))
		return FAILED;

	if( (mTLSHandshakeStatus == eSSLHandshakeInProgress) ||
			(mTLSHandshakeStatus == eSSLHandshakePending)) {
		/*
		 * The caller will have to repeat this operation
		 */
		return INPROGRESS;
	}

	if(mTLSHandshakeStatus == eSSLHandshakeDone) {
		/*
		 * Proceed to PushFYI Handshake only after TLS Handshake is completed
		 */
		return doPushFYIHandshake();
	}

	return FAILED;
}

PushFYIClient::TASK_STATUS TLSClient::doPushFYIHandshake()
{
	/*
	 * Must ensure SSL Handshake Complete Before PushFYI Handshake
	 */
	if(mTLSHandshakeStatus!=eSSLHandshakeDone) {
		return FAILED;
	}

	rxInt fd = getSocket();

	bool isHTTPRequest = false, isHTTPPostRequest = false;

	char request[4096] = {0};
	char response[4096] = {0}, sha1[29] = {0};

	//rxInt ret = 0;

	if(mHandshakeStatus == ePushFYIHandshakeReadPending) {
		ssize_t count = 0;

	    /*
	     * Lets read the handshake in size of 4 KBs
	     * 1KB = 1024 Bytes
	     * 4KB = 1024 * 4 Bytes
	     */
		while(true) {
	    	/*
	    	 * Empty the buffer
	    	 */
	    	memset(request, 0, sizeof(request));

			count = SSL_read(mSSL, request, 4096);

			if(count < 0) {
				switch(SSL_get_error(mSSL, count)) {
					case SSL_ERROR_NONE:
					{
						INFO(Log::eNetwork, "SSL_read PushFYI handshake read complete on fd = %d", fd);
						mHandshakeStatus = ePushFYIHandshakeWritePending;
					}
					break;

					case SSL_ERROR_ZERO_RETURN:
					{
						INFO(Log::eNetwork, "SSL_read Client closed during PushFYI handshake on fd = %d", fd);
						mHandshakeStatus = ePushFYIHandshakeTerminated;
					}
					break;

					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_READ:
					{
						INFO(Log::eNetwork, "SSL_read PushFYI handshake is in progress on fd = %d", fd);
						mHandshakeStatus = ePushFYIHandshakeReadPending;
					}
					break;

					case SSL_ERROR_SYSCALL:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Syscall error during PushFYI Handshake on fd = %d %s",count, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						mHandshakeStatus = ePushFYIHandshakeFailed;
					}
					break;

					case SSL_ERROR_SSL:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Critical error during PushFYI Handshake on fd = %d %s",count, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						mHandshakeStatus = ePushFYIHandshakeFailed;
					}
					break;

					default:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Unknown error during PushFYI Handshake on fd = %d, %s", count, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						mHandshakeStatus = ePushFYIHandshakeFailed;
					}
				}
				break;
			}

			if(count == 0) {
				INFO(Log::eNetwork, "SSL_read Client closed during PushFYI handshake on fd = %d", fd);
				mHandshakeStatus = ePushFYIHandshakeTerminated;

				break;
			}

			INFO(Log::eNetwork, "Total bytes read = %d", count);

			if(mHandshakeBufferOffset >= 4096*2) {
				/*
				 * Header can not be this long.
				 * Possible someone fooling around.
				 *
				 * Terminate
				 */
				mHandshakeStatus = ePushFYIHandshakeTerminated;
				break;
			}

			memcpy(mHandshakeBuffer+mHandshakeBufferOffset, request, count);
			mHandshakeBufferOffset += count;
		}

		/*
		 * Check the status of non blocking ssl io operation
		 */
		if( (mHandshakeStatus == ePushFYIHandshakeTerminated) ||
				(mHandshakeStatus == ePushFYIHandshakeFailed)) {
			INFO(Log::eNetwork, "Client terminated handshake on fd = %d", fd);
			return FAILED;
		}

		/*
		 * Handshake has been read
		 * we have to write the response now.
		 */
		if( (mHandshakeStatus == ePushFYIHandshakeWritePending) ||
				(mHandshakeStatus == ePushFYIHandshakeReadPending)) {
			/*
			 * All data has been read.
			 * Check to see if we have EOF
			 */
			if (strstr(mHandshakeBuffer, "\r\n\r\n")) {
				/*
				 * We have the end of message.
				 */
				mHandshakeStatus = ePushFYIHandshakeWritePending;
				//mHandshakeBufferOffset = 0;

				INFO(Log::eNetwork, "Handshake read complete on fd = %d, Handshake = \n%s", fd, mHandshakeBuffer);

			} else {
				/*
				 * In complete data, lets return INPROGRESS and
				 * wait for more data to arrive.
				 */
				mHandshakeStatus = ePushFYIHandshakeReadPending;
			}
		}

		if( (mHandshakeStatus == ePushFYIHandshakeReadPending) ) {
			/*
			 * Caller will have to repeat this operation.
			 */
			INFO(Log::eNetwork, "Partial handshake read on fd = %d, Handshake = \n%s", fd, mHandshakeBuffer);
			return INPROGRESS;
		}
	}

	if(mHandshakeStatus == ePushFYIHandshakeWritePending) {
		/*
		 * Check if it is a valid HTTP REQUEST.
		 * This is a very trivial check.
		 */
		char* handshake = mHandshakeBuffer;

		if (strstr(handshake, "\r\n\r\n"))
			isHTTPRequest = true;
		else {
			INFO(Log::eNetwork, "Doesn't seem like a HTTP Request \n%s\n", handshake);
			return FAILED;
		}

		if(isHTTPRequest) {
			/*
			 * Parse the handshake for a WebSocket Header and
			 * negotiate a protocol
			 */
			INFO(Log::eNetwork, "Parsing Pushfyi handshake request");

			P_PUSHFYI_HEADER	pHeader = &mHeader;
		    char* start, *end;

		    pHeader->key1[0] = '\0';
		    pHeader->key2[0] = '\0';
		    pHeader->key3[0] = '\0';

		    /*
		     * WebSocket Protocols must initiate with an HTTP GET Request.
		     */
		    if (strlen(handshake) < 92) {
		        ERROR(Log::eProtocol, "invalid header length on fd = %d", fd);
		        mProtocol =  eProtocolInvalidHeader;

		        return FAILED;
		    }

		    else if(bcmp(handshake, "POST ", 5) == 0) {
		    	INFO(Log::eProtocol, "Event source header detected on fd = %d", fd);
		    	isHTTPPostRequest = true;
		    } else if(bcmp(handshake, "GET ", 4) == 0) {
		    	INFO(Log::eProtocol, "HTTP Client header detected on fd = %d", fd);
		    	isHTTPPostRequest = false;
		    } else {
		        ERROR(Log::eProtocol, "Invalid request header on fd = %d", fd);
		        mProtocol =  eProtocolInvalidHeader;
		        return FAILED;
		    }

		    if(!isHTTPPostRequest) {
		    	mHandshakeBufferOffset = 0;

		    	/*
		    	 * Parse the HTTP GET Request
		    	 */
		    	start = handshake + 4;

				/*
				 * Ensure HTTP 1.1
				 */
				end = strstr(start, " HTTP/1.1");
				if (!end) { return FAILED; }
				strncpy(pHeader->path, start, end-start);
				pHeader->path[end-start] = '\0';

				/*
				 * Capture HOST address
				 */
				start = strstr(handshake, "\r\nHost: ");
				if (!start) { return FAILED; }
				start += 8;
				end = strstr(start, "\r\n");
				strncpy(pHeader->host, start, end-start);
				pHeader->host[end-start] = '\0';

				/*
				 * Capture User Agent
				 */
				start = strstr(handshake, "\r\nUser-Agent: ");
				if (!start) { return FAILED; }
				start += 14;
				end = strstr(start, "\r\n");
				strncpy(pHeader->agent, start, end-start);
				pHeader->agent[end-start] = '\0';

				/*
				 * Capture Origin address
				 */
				pHeader->origin[0] = '\0';
				start = strstr(handshake, "\r\nOrigin: ");
				if (start) {
					start += 10;
				} else {
					start = strstr(handshake, "\r\norigin: ");
					if(start) {
						start += 10;
					} else {
						start = strstr(handshake, "\r\nSec-WebSocket-Origin: ");
						if (!start) { return FAILED; }
						start += 24;
					}
				}
				end = strstr(start, "\r\n");
				strncpy(pHeader->origin, start, end-start);
				pHeader->origin[end-start] = '\0';

				/*
				 * Capture websocket version string
				 */
				start = strstr(handshake, "\r\nSec-WebSocket-Version: ");
				if(start) {
					/*
					 *  HyBi / RFC 6455
					 */
					start += 25;
					end = strstr(start, "\r\n");
					strncpy(pHeader->version, start, end-start);
					pHeader->version[end-start] = '\0';

					start = strstr(handshake, "\r\nSec-WebSocket-Key: ");
					if (!start) { return FAILED; }
					start += 21;
					end = strstr(start, "\r\n");
					strncpy(pHeader->key1, start, end-start);
					pHeader->key1[end-start] = '\0';

					start = strstr(handshake, "\r\nConnection: ");
					if (!start) { return FAILED; }
					start += 14;
					end = strstr(start, "\r\n");
					strncpy(pHeader->connection, start, end-start);
					pHeader->connection[end-start] = '\0';

					start = strstr(handshake, "\r\nSec-WebSocket-Protocol: ");
					if (!start) { return FAILED; }
					start += 26;
					end = strstr(start, "\r\n");
					strncpy(pHeader->protocols, start, end-start);
					pHeader->protocols[end-start] = '\0';

					//return eProtocolPushfyiHyBi;
					mProtocol = eProtocolWebSocket;
				}

				else {
					/*
					 *  Hixie 75 or 76
					 */
					INFO(Log::eProtocol, "Possibly Hixie 75/76 on fd = %d, we dont support it.", fd);
					mProtocol = eProtocolUnknown;
				}
		    }

		    else {
		    	/*
		    	 * Parse the HTTP POST Request
		    	 */
		    	start = handshake + 5;

				/*
				 * Ensure HTTP 1.1
				 */
				end = strstr(start, " HTTP/1.1");
				if (!end) { return FAILED; }
				strncpy(pHeader->path, start, end-start);
				pHeader->path[end-start] = '\0';

				/*
				 * Capture HOST address
				 */
				start = strstr(handshake, "\r\nHost: ");
				if (!start) { return FAILED; }
				start += 8;
				end = strstr(start, "\r\n");
				strncpy(pHeader->host, start, end-start);
				pHeader->host[end-start] = '\0';

				/*
				 * Origin is not important for now.
				 *
				 * But shall be later.
				 */

				/*
				 * Capture Connection
				 */
				start = strstr(handshake, "\r\nConnection: ");
				if (!start)
				{
					INFO(Log::eProtocol, "Client on fd = %d does not support http streaming", fd);
				}

				else {
					start += 14;
					end = strstr(start, "\r\n");
					strncpy(pHeader->connection, start, end-start);
					pHeader->connection[end-start] = '\0';
				}

				/*
				 * Capture content type
				 */
				start = strcasestr(handshake, "\r\nContent-Type: ");
				if (!start)
				{
					INFO(Log::eProtocol, "Client on fd = %d does not specify content type", fd);
					return FAILED;
				}

				else {
					start += 16;
					end = strstr(start, "\r\n");
					strncpy(pHeader->content, start, end-start);
					pHeader->content[end-start] = '\0';
				}

				/*
				 * Capture content length
				 */
				start = strcasestr(handshake, "\r\nContent-Length: ");
				if (!start)
				{
					INFO(Log::eProtocol, "Client on fd = %d does not specify content type", fd);
					return FAILED;
				}

				else {
					start += 18;
					end = strstr(start, "\r\n");
					strncpy(pHeader->length, start, end-start);
					pHeader->length[end-start] = '\0';
				}

				rxUInt contentLength = atoi(pHeader->length);

				if(mHandshakeBufferOffset < contentLength+92) {
					INFO(Log::eProtocol, "More data expected");
					mHandshakeStatus = ePushFYIHandshakeReadPending;
					return INPROGRESS;
				}
				else {
					mHandshakeStatus = ePushFYIHandshakeWritePending;
					mProtocol = eProtocolEventSource;
				}
		    }

		    if( !isHTTPPostRequest && (mProtocol == eProtocolWebSocket) ) {
		    	/*
		    	 * Let's compare the protocol strings supported.
		    	 */

		    	if(strcmp(pHeader->protocols, WEBSOCKET_PROTOCOL) == 0) {
		    		INFO(Log::eProtocol, "pushfyi compliant websocket protocol on fd = %d", fd);

		    		ProtocolHelper::gen_sha1(pHeader, sha1);
			        sprintf(response, SERVER_HANDSHAKE_HYBI, sha1, WEBSOCKET_PROTOCOL);

		    	} else if(strcmp(pHeader->protocols, STREAMSOCKET_PROTOCOL) == 0) {
		    		INFO(Log::eProtocol, "pushfyi compliant streamsocket protocol on fd = %d", fd);

			        ProtocolHelper::gen_sha1(pHeader, sha1);
			        sprintf(response, SERVER_HANDSHAKE_HYBI, sha1, STREAMSOCKET_PROTOCOL);

			        /*Switch to Stream Socket*/
			        mProtocol = eProtocolStreamSocket;

		    	} else {
		    		/*
		    		 * Discard nything else.
		    		 *
		    		 * We are very very strict. You cant mess with us.
		    		 */
		    		INFO(Log::eProtocol, "unknown protocol string on fd = %d. Discarding connection", fd);
		    		return FAILED;
		    	}

		    	/*
		    	 * Lets send the websocket upgrade response
		    	 */
		    	INFO(Log::eNetwork, "Sending handshake response to client on fd = %d\n%s", fd, response);

		    	size_t toWrite = strlen(response);
		    	while(mHandshakeBufferOffset < toWrite) {
		    		ssize_t written = SSL_write(mSSL, response+mHandshakeBufferOffset, toWrite-mHandshakeBufferOffset);
		    		if(written < 0)
		    		{
						/*
						 * Lets check the error
						 */
						switch(SSL_get_error(mSSL, written)) {
							case SSL_ERROR_NONE:
							{
								INFO(Log::eNetwork, "SSL_write PushFYI handshake response sent on fd = %d", fd);
								mHandshakeStatus = ePushFYIHandshakeDone;
							}
							break;

							case SSL_ERROR_ZERO_RETURN:
							{
								INFO(Log::eNetwork, "SSL_write Client closed during PushFYI handshake response on fd = %d", fd);
								mHandshakeStatus = ePushFYIHandshakeTerminated;
							}
							break;

							case SSL_ERROR_WANT_WRITE:
							case SSL_ERROR_WANT_READ:
							{
								/*
								 * SSL Renegotiation may be in progress.
								 */
								INFO(Log::eNetwork, "SSL_write PushFYI handshake response write in progress on fd = %d", fd);
								mHandshakeStatus = ePushFYIHandshakeWritePending;
							}
							break;
							case SSL_ERROR_SSL:
							{
								INFO(Log::eNetwork, "SSL_write Critical Error during PushFYI Handshake response on fd = %d", fd);
								mHandshakeStatus = ePushFYIHandshakeFailed;
							}
							break;

							default:
							{
								INFO(Log::eNetwork, "SSL_read Unknown error during PushFYI Handshake on fd = %d, %s", fd,
										ERR_error_string(ERR_get_error(), 0));
								mHandshakeStatus = ePushFYIHandshakeFailed;
							}
						}

						break;
		    		}

					/*
					 * written bytes were written in this call
					 */
					mHandshakeBufferOffset+=written;

					if(mHandshakeBufferOffset>=toWrite)
					{
						mHandshakeStatus = ePushFYIHandshakeDone;
						break;
					}
		    	}

		        /*
		         * Check the status once again
		         */
				if( (mHandshakeStatus == ePushFYIHandshakeTerminated) ||
						(mHandshakeStatus == ePushFYIHandshakeFailed))
					return FAILED;

				/*
				 * TODO:
				 * This check seems unnecessary right now
				 */
				if( (mHandshakeStatus == ePushFYIHandshakeWritePending) ||
						(mHandshakeStatus == ePushFYIHandshakeWritePending)) {
					/*
					 * Caller will have to repeat this operation.
					 */
					return INPROGRESS;
				}

				/*
				 * Handshake response has been sent
				 */
				if(mHandshakeStatus == ePushFYIHandshakeDone)
				{
					/*
					 * We have handshaked.
					 * Lets create a frame decoder based on Client Protocol
					 */

					INFO(Log::eProtocol, "Creating a frame decoder for client on fd = %d", fd);

					switch (mProtocol)
					{
						case eProtocolWebSocket:
						{
							PushFYIProtocolFactory* factory = PushFYIProtocolFactory::getInstance();
							mCodec = factory->CreateClientCodec("websocket");
						}
						break;

						case eProtocolStreamSocket:
						{
							PushFYIProtocolFactory* factory = PushFYIProtocolFactory::getInstance();
							mCodec = factory->CreateClientCodec("stream");
						}
						break;

						default:
						{
							mCodec = 0;
						}
						break;
					}

					if(mCodec == 0) {
						ERROR(Log::eProtocol, "This is an unsupported protocol");
						return FAILED;
					}

					else
						INFO(Log::eNetwork, "Codec created for client on fd = %d", fd);

					return COMPLETED;
				} else {
					/*
					 * Unknown handshake status
					 */
					return FAILED;
				}
		    } else if(isHTTPPostRequest && (mProtocol == eProtocolEventSource)) {

		    	INFO(Log::eProtocol, "pushfyi compliant event source on fd = %d", fd);

		    	//mHandshakeStatus = ePushFYIHandshakeDone;

				mHandshakeStatus = ePushFYIHandshakeReadPending;

		    	/*
		    	 * An event source could connect to us
		    	 * to publish a message.
		    	 *
		    	 * Lets ensure the right content type and message
		    	 */

		    	rxUInt contentLength = atoi(pHeader->length);

		    	if( (strcmp(pHeader->content, EVENT_SOURCE_CONTENT_TYPE_XML) == 0) ||
		    			(strcmp(pHeader->content, EVENT_SOURCE_CONTENT_TYPE_FORM) == 0)	)
		    	{
		    		/*
		    		 * Lets read the content
		    		 */
		    		start = strstr(handshake, "\r\n\r\n");
		    		if(start) {

		    			char postdata[MAX_PAYLOAD_SIZE] = {0};

		    			memset(postdata, 0, sizeof(postdata));
		    			strncpy(postdata, start+4, contentLength);

		    			INFO(Log::eProtocol, "Event source sent post data = \n%s", postdata);

				    	char* source = postdata;
				    	char* start = strstr(source, "<ims-event>");
				    	char* end = strstr(source, "</ims-event>");

				    	while(start && end)
				    	{
				    		string msg(start, end+11);
				    		Event e = Event::toEvent(msg);

				    		source = end+12;

				    		start = strstr(source, "<ims-event>");
					    	end = strstr(source, "</ims-event>");

					    	mRouter.publish(e);

					    	//update stat
					    	Event stat = new EventRep(PUSHFYI_MANAGER_TOPIC, "stat");
					    	stat->set("command", "messages");
					    	stat->setUInt("value", 1);
					    	mRouter.publish(stat);
				    	}
				    	//send a response
				    	char data[1024] = {0};
				    	strcpy(data, "{\"error\": \"false\", \"status\": \"200\"}");
				    	sprintf(response, HTTP_POST_EVENT_SOURCE_RESPONSE, strlen(data), data);
				    	SSL_write(mSSL, response, strlen(response));

				        memset(mHandshakeBuffer, 0, sizeof(mHandshakeBuffer));
				        mHandshakeBufferOffset = 0;

				    	INFO(Log::eProtocol, "HTTP Response sent on to event source on fd = %d", fd);
						return COMPLETED;
		    		} else {
			    		ERROR(Log::eProtocol, "Invalid content from event source on fd = %d", fd);
			    		return FAILED;
		    		}
		    	}

		    	else {
		    		ERROR(Log::eProtocol, "Invalid content type from event source on fd = %d", fd);
		    		return FAILED;
		    	}
		    }  else {
		    	/*
		    	 * This is some extra terrestrial entity
		    	 */
		    	return FAILED;
		    }
		}
		/*
		 * Not an HTTP Upgrade Request
		 */
		return FAILED;
	}
	/*
	 * Not a valid Handshake Status
	 */
	return FAILED;
}

/**
 *
 */
PushFYIClient::TASK_STATUS TLSClient::read(Event& ev)
{
	Synchronized lock(mMutex);

	rxInt fd = getSocket();

	TASK_STATUS ret = FAILED;

	if(mHandshakeStatus == ePushFYIHandshakeDone) {

		/*
		 * Lets read everything we got.
		 */
	    ssize_t count = 0;
	    size_t read = 0;

	    /*
	     * Lets read the XML in size of 4 KBs
	     * 1KB = 1024 Bytes
	     * 4KB = 1024 * 4 Bytes
	     */
	    const ssize_t chunk = 4 * 1024;
	    char buffer[chunk] = {0};

	    char encoded[MAX_PAYLOAD_SIZE] = {0};
	    memset(encoded, 0, sizeof(encoded));

	    /*
	     * Check if we have any pending read before proceeding to read
	     */
	    if(mReadPendingOffset > 0) {
	    	//copy the buffer to encoded buffer
	    	memcpy(encoded, mReadPending, mReadPendingOffset);
	    	read = mReadPendingOffset;

	    	memset(mReadPending, 0, sizeof(mReadPendingOffset));
	    	mReadPendingOffset = 0;
	    }

	    while(true) {
	    	/*
	    	 * Empty the buffer
	    	 */
	    	memset(buffer, 0, sizeof(buffer));

	    	count = SSL_read(mSSL, buffer, chunk);
			if(count < 0) {
				switch(SSL_get_error(mSSL, ret)) {
					case SSL_ERROR_NONE:
					{
						INFO(Log::eNetwork, "SSL_read complete on fd = %d", fd);
						ret = COMPLETED;

						/*
						 * Reset the Pending Read buffer
						 */
						memset(mReadPending, 0, sizeof(mReadPending));
						mReadPendingOffset = 0;
					}
					break;

					case SSL_ERROR_ZERO_RETURN:
					{
						INFO(Log::eNetwork, "SSL_read Client closed on fd = %d", fd);
						ret = FAILED;
					}
					break;

					case SSL_ERROR_WANT_WRITE:
					case SSL_ERROR_WANT_READ:
					{
						INFO(Log::eNetwork, "SSL_read is in progress on fd = %d", fd);
						ret = INPROGRESS;
					}
					break;

					case SSL_ERROR_SYSCALL:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Syscall error during PushFYI Handshake on fd = %d %s",ret, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						ret = FAILED;
					}
					break;

					case SSL_ERROR_SSL:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Critical error during PushFYI Handshake on fd = %d %s",ret, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						ret = FAILED;
					}
					break;

					default:
					{
						INFO(Log::eNetwork, "SSL_read returned %d Unknown error during PushFYI Handshake on fd = %d, %s", ret, fd,
								ERR_error_string(ERR_get_error(), 0));

						ERROR(Log::eNetwork, "Errno = %d, Error = %s", errno, strerror(errno));
						ret = FAILED;
					}
					break;
				}
				break;
			}

			if( count == 0 ) {
				/*
				 * Client terminated during read
				 */
				ERROR(Log::eNetwork, "Client terminated the connection");
				ret = FAILED;
				break;
			}

			if(count+read >= MAX_PAYLOAD_SIZE) {
				/*
				 * We will run out of buffer.
				 *
				 * We have to stop reading. and backup the buffer
				 */
				INFO(Log::eNetwork, "Buffer size exceed prevented");
				memcpy(mReadPending, buffer, count);
				mReadPendingOffset = count;

				ret = INPROGRESS;
				break;
			}


			memcpy(encoded+read, buffer, count);
			read += count;
	    }

	    /*
	     * We are done reading. Lets check of reading status
	     */
	    if(ret == FAILED) {
	    	/*
	    	 * Lets kill this client
	    	 */
	    	return ret;
	    } else {
	    	/*
	    	 * Before we decode, lets see if we have any bytes
	    	 * pending for decode.
	    	 */
	    	char* buffer = 0;
	    	if(mDecodePendingOffset > 0) {
	    		buffer = new char[read + mDecodePendingOffset];

	    		memset(buffer, 0, read + mDecodePendingOffset);

	    		memcpy(buffer, mDecodePending, mDecodePendingOffset);
	    		memcpy(buffer+mDecodePendingOffset, encoded, read);

	    		memset(mDecodePending, 0, sizeof(mDecodePending));

	    		read += mDecodePendingOffset;
	    		mDecodePendingOffset = 0;
	    	} else {
	    		buffer = new char[read];

	    		memset(buffer, 0, read);
	    		memcpy(buffer, encoded, read);
	    	}

	    	INFO(Log::eNetwork, "bytes read = %d \n%s", read, buffer);
	    	//INFO(Log::eNetwork, "bytes read = %d", read);

	    	char payload[MAX_PAYLOAD_SIZE] = {0};
	    	rxUInt bytesProcessed = 0;

	    	/*
	    	 * List if events to publish
	    	 */
	    	EventList list = new EventListRep("ReadEvents");

	    	while(bytesProcessed < read) {

	    		memset(payload, 0, sizeof(payload));

	    		int opcode = mCodec->decode(buffer+bytesProcessed, read-bytesProcessed, payload, bytesProcessed);

	    		switch(opcode) {
					case -1: {
						/*
						 * Not enough data.
						 */
						ret = COMPLETED;

						memcpy(mDecodePending, buffer+bytesProcessed, read-bytesProcessed);
						mDecodePendingOffset = read-bytesProcessed;
						//memset(mDecodePending, 0, sizeof(mDecodePending));
						//mDecodePendingOffset = 0;

						INFO(Log::eNetwork, "Decode Completed.");
					}
					break;

	    			case -2: {
	    				/*
	    				 * Not enough data.
	    				 */
	    				ret = INPROGRESS;

						memcpy(mDecodePending, buffer+bytesProcessed, read-bytesProcessed);
						mDecodePendingOffset = read-bytesProcessed;

						INFO(Log::eNetwork, "Bytes pending for decode %d", mDecodePendingOffset);

	    			}
	    			break;

	    			case 0x0: {
	    				/*
	    				 * This is continuation data
	    				 */
	    				memcpy(mContinuationData+mContinuationDataOffset, payload, strlen(payload));
	    				mContinuationDataOffset = strlen(payload);
	    			}
	    			break;

					case 0x1: {
						if(mContinuationDataOffset > 0) {
							/*
							 * Concat this data
							 */
							memcpy(mContinuationData+mContinuationDataOffset, payload, strlen(payload));
						} else {
							memcpy(mContinuationData, payload, strlen(payload));
						}
					}
					break;

					case 0x9: {
						bytesProcessed = read;
					}
					break;

					case 0xA: {
						bytesProcessed = read;
					}
					break;

					case -3:
					case 0x8: {
						INFO(Log::eNetwork, "terminating client connection on fd = %d", fd);
						ret = FAILED;
						bytesProcessed = read;
					}
					break;
	    		} //end switch

	    		INFO(Log::eNetwork, "bytes processed = %d, opcode = %d", bytesProcessed, opcode);

	    		if(bytesProcessed == 0) {
	    			INFO(Log::eNetwork, "There was nothing meaningful to process. Ignoring.");
	    		}

	    		if(opcode == -2 || opcode == -1 || bytesProcessed == 0) {
	    			break;
	    		}

	    		if(opcode == 1) {
	    			/*
	    			 * mContinuationData contains one XML
	    			 */
	    			Event event = Event::toEvent(mContinuationData);
	    			INFO(Log::eNetwork, "Event found \n%s", event->toString().c_str());

	    			/*
	    			if(event->get("command") == "pong") {

	    				 // This is an application layer ping.
	    				 // send a pong and notify we are here.

	    				mPongReceived = true;
	    				mPingSent = false;
	    				INFO(Log::eNetwork, "PONG received on fd = %d", fd);
	    			}
	    			else {
	    				list->addEvent(event);
	    			}
	    			*/
	    			list->addEvent(event);

	    			/*
	    			 * clear the buffer
	    			 */
	    			memset(mContinuationData, 0, sizeof(mContinuationData));
	    			mContinuationDataOffset = 0;
	    		}

	    		if(opcode == 0x9) {
	    			/*
	    			 * Send a PONG
	    			 */
			    	/*
			    	 * We must send a PONG.
			    	 *
			    	 * mDataBuffer contains the application data
			    	 */
			    	char pong[10] = {0};
			    	rxInt length = strlen(payload);

			    	pong[0] = 138;
			    	pong[1] = length;

			    	memcpy(pong+2, payload, length);
			    	SSL_write(mSSL, pong, length+2);

			    	INFO(Log::eNetwork, "PONG sent on fd = %d", fd);
	    		}

	    		if(opcode == 0xA) {
			    	/*
			    	 * This is an unsolicited PONG.
			    	 * Dont do anything.
			    	 */
			    	INFO(Log::eNetwork, "Possibly an unsolicited heartbeat. do nothing.");
	    		}
	    	} //end while

	    	if(list->size() > 0)
	    		ev = list;

	    	if(buffer) {
	    		delete [] buffer;
	    	}

	    	if(bytesProcessed >=  read) {
				memset(mDecodePending, 0, sizeof(mDecodePending));
				mDecodePendingOffset = 0;
	    	}

	    }
	}

	return ret;
}

/*
 * This function may return INPROGRESS when SSL_write
 * could not complete the write operation.
 *
 * When INPROGRESS is returned, the caller must call
 * this function with the same buffer.
 */
PushFYIClient::TASK_STATUS TLSClient::write(Event& ev)
{
	Synchronized lock(mMutex);

	rxInt fd = getSocket();
	TASK_STATUS ret = COMPLETED;

	/*
	 * Lets send the data
	 */
	char content[MAX_PAYLOAD_SIZE] = {0};
	char encoded[MAX_PAYLOAD_SIZE] = {0};

	memset(content, 0, sizeof(content));
	memset(encoded, 0, sizeof(encoded));

	/*
	 * Lets copy the buffer
	 */
	memcpy(content, ev->toString().c_str(), strlen(ev->toString().c_str()));

	/*
	 * Lets encode the data
	 */
	size_t toWrite = 0; size_t written = 0;
	if(mCodec) {
		toWrite = mCodec->encode(content, encoded);
	}

	while(written < toWrite) {
		ssize_t count = SSL_write(mSSL, encoded, toWrite);

		if(count > 0) {
			written += count;

			ret = COMPLETED;
			if(written < toWrite)
				INFO(Log::eNetwork, "SSL_write wrote = %d bytes on fd = %d", written, fd);
			break;
		}

		else if(count == 0) {
			ret = FAILED;

		    long error = ERR_get_error();
		    const char* error_str = ERR_error_string(error, NULL);
		    printf("SSL_write failed (returned 0): %s\n", error_str);

		    break;
		}
		else if(count < 0)
		{
			/*
			 * Lets check the error
			 */
			switch(SSL_get_error(mSSL, count)) {
				case SSL_ERROR_NONE:
				{
					INFO(Log::eNetwork, "SSL_write response sent on fd = %d", fd);
					ret = COMPLETED;
				}
				break;

				case SSL_ERROR_ZERO_RETURN:
				{
					INFO(Log::eNetwork, "SSL_write Client closed on fd = %d", fd);
					ret = FAILED;
				}
				break;

				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_READ:
				{
					/*
					 * SSL Renegotiation may be in progress.
					 */
					INFO(Log::eNetwork, "SSL_write in progress on fd = %d", fd);
					ret = INPROGRESS;
				}
				break;
				case SSL_ERROR_SSL:
				{
					INFO(Log::eNetwork, "SSL_write Critical Error on fd = %d", fd);
					ret = FAILED;
				}
				break;

				default:
				{
					INFO(Log::eNetwork, "SSL_write Unknown error on fd = %d, %s", fd,
							ERR_error_string(ERR_get_error(), 0));
					ret = FAILED;
				}
			}

			break;
		}
	}

	return ret;
}

/**
 *
 * Process method process the Client's Pending
 * Messages in his PublishQueue.
 *
 * However this processing can starve other client's
 * if the QueueSize is considerably high.
 *
 * For this reason we should do batching, i.e.
 * Process no more than MAX_NO_CMD_TO_PROCESS in
 * one go.
 */
PushFYIClient::TASK_STATUS TLSClient::process()
{
    static const rxInt MAX_NO_CMD_TO_PROCESS = 4;
    bool moreCmdsToProcess = false;

    rxInt n = mPublishQueue->getEntryCount();

    INFO(Log::eNetwork, "Found %d commands in queue for client on fd = %d", n, getSocket());

    if(n > MAX_NO_CMD_TO_PROCESS) {
        n = MAX_NO_CMD_TO_PROCESS;
        moreCmdsToProcess = true;
    }

    Event ev = NULL;

    TASK_STATUS ret = COMPLETED;
    for(rxInt i = 0; i < n; i++) {
		ev = mPublishQueue->pop();

        /*
         * Lets see if this is an internal
         * signal.
         */
        if( (ev->getTopic() == getLicenseKey()) &&
        		(ev->getSubtopic() == getSignalingKey())	)
        {
        	ret = processInternalSignal(ev);

        	if( (ret == COMPLETED) && (ev->get("command") == "authorize") ) {
        		Event res = new EventRep(PUSHFYI_MANAGER_TOPIC, "authorizecomplete");
        		res->set("command", "authorizecomplete");

        		this->publish(res);
        	}
        }

        if(ev->get("command") == "kill") {
        	ret = FAILED;
        }

        else {
        	/*
        	 * Wrie this to the socket
        	 */
        	ret = write(ev);
        }

        if(ret == FAILED)
        	break;

        if(ret == INPROGRESS)
        	break;
    }

    if(ret == COMPLETED)
    {
    	INFO(Log::eNetwork, "Write returned = %d", ret);
		if( moreCmdsToProcess ) {
			notify();
			INFO(Log::eNetwork, "Publish Queue processed partially for client on fd = %d", getSocket());
		}

		else {
			unnotify();
			INFO(Log::eNetwork, "Publish Queue processed for client on fd = %d", getSocket());
		}
    }

    if(ret == INPROGRESS) {
    	INFO(Log::eNetwork, "Write returned = %d", ret);

    	mPublishQueue->pushFront(ev);

    	notify();
    	INFO(Log::eNetwork, "Pending write data is queued again for client on fd = %d.", getSocket());
    }

    return ret;
}

int TLSClient::ping()
{
	Synchronized lock(mMutex);
	rxInt fd = getSocket();

	/*
	 * Client is idle. Lets ping it.
	 *
	 * This shall be a Application Layer Ping
	 * and NOT RFC6455 PING
	 */

	if(!mPingSent) {
		/*
		 * Send a PING
		 */
		char ping[256] = {0};
		size_t toWrite = 0;
		if(mCodec) {
			toWrite = mCodec->encode("-ws-keepalive-", ping);
		}
		SSL_write(mSSL, ping, toWrite);

		INFO(Log::eNetwork, "PING sent on fd = %d, Bytes written = %d", fd, toWrite);

		mPingSent = true;
		mPongReceived = false;

		//wait for PONG
		startHeartBeatTimer(20*1000);
	}

	else {
		/*
		 * PONG Wait timed out
		 */
		if(mPongReceived) {
			/*
			 * We have a PONG in time
			 */
			mPongReceived = false;
			mPingSent = false;

			mHeartbeatsMissed = 0;

			//send PING on next expiry
			startHeartBeatTimer(20*1000);
		}

		else {
			/*
			 * We have missed a heartbeat
			 */
			mHeartbeatsMissed++;

			if(mHeartbeatsMissed > 5) {
				/*
				 * Terminate this client.
				 * We are done WAITING!!
				 */
				INFO(Log::eNetwork, "Connection lost from fd = %d, disconnecting.", fd);

				//lets kill ourselves
				Event kill = new EventRep(getAppSecret(), getSignalingKey());
				kill->set("command", "kill");

				submitPublication(kill);
			}

			else {
				/*
				 * We will wait some more
				 */
				INFO(Log::eNetwork, "Missed heartbeat from fd = %d", fd);
				startHeartBeatTimer(20*1000);
			}

		}
	}

	return 0;
}
