/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Client Socket Factory. Creates client socket
based on protocol and security scheme.

*********************************************************
*/

#include "PushFYIProtocolFactory.h"
#include "Protocol-Defs.h"
#include "WebSocket.h"
#include "StreamSocket.h"

PushFYIProtocolFactory* PushFYIProtocolFactory::mInstance = 0;

PushFYIProtocolFactory* PushFYIProtocolFactory::getInstance()
{
	if(mInstance == 0) {
		mInstance = new PushFYIProtocolFactory();
	}
	return mInstance;
}

void PushFYIProtocolFactory::RegisterPushFYIProtocols()
{
	/*
	 * 1. WebSocket Protocol
	 * 		RFC 6455 Compliant
	 * 		Clients: Browsers
	 *
	 * 		Protocol String: "websocket"
	 */
	mPushFYICodecsMap.insert(std::pair<string, create_codec>
							(WEBSOCKET_PROTOCOL, WebSocket::CreateCodec));

	/*
	 * 2. Stream Socket Protocol
	 * 		Plain Stream Socket Protocol
	 * 		Clients: Native Apps
	 *
	 * 		Protocol String: "stream"
	 */
	mPushFYICodecsMap.insert(std::pair<string, create_codec>
							(STREAMSOCKET_PROTOCOL, StreamSocket::CreateCodec));

}

PushFYIProtocolFactory::PushFYIProtocolFactory()
{
	RegisterPushFYIProtocols();
}

PushFYIProtocolFactory::~PushFYIProtocolFactory()
{

}

/*
 * Creates Pushfyi Protocol Codec
 *
 * @param protocolString Protocol string as received in the HTTP Upgrade request
 *
 * @return Pointer to the instance of PushFYICodec
 */
PushFYICodec* PushFYIProtocolFactory::CreateClientCodec(string protocolString)
{
	PushFYICodec* codec = 0;

	PUSHFYI_CODECS_MAP_ITER iter = mPushFYICodecsMap.begin();
	iter = mPushFYICodecsMap.find(protocolString);

	if(iter != mPushFYICodecsMap.end()) {
		create_codec func = iter->second;
		codec = (*func)();
	}

	return codec;
}
