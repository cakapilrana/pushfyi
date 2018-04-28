/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Pushfyi Compatible Protocol's Factory Class
*********************************************************
*/

#ifndef __PROTOCOL_FACTORY__
#define __PROTOCOL_FACTORY__

#include <map>
#include "PushFYICodec.h"

using namespace std;

class PushFYIProtocolFactory {
	typedef PushFYICodec* (*create_codec) ();
	typedef std::map<string, create_codec>	PUSHFYI_CODECS_MAP;
	typedef std::map<string, create_codec>::iterator	PUSHFYI_CODECS_MAP_ITER;
private:
	PushFYIProtocolFactory();
	PushFYIProtocolFactory(const PushFYIProtocolFactory&);

	static PushFYIProtocolFactory* mInstance;

	PUSHFYI_CODECS_MAP	mPushFYICodecsMap;

	void RegisterPushFYIProtocols();

public:
	virtual ~PushFYIProtocolFactory();

	static PushFYIProtocolFactory* getInstance();
	PushFYICodec* CreateClientCodec(string clientType);
};

#endif //__PROTOCOL_FACTORY__
