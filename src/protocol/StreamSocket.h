/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: RFC 6455 Compliant WebSocket Protocol.

This Class Handles The RFC 6455 BASE FRAMING PROTOCOL
*********************************************************
*/
#ifndef __STREAMSOCKET_PROTOCOL__
#define __STREAMSOCKET_PROTOCOL__

#include "Protocol-Defs.h"
#include "PushFYICodec.h"

class StreamSocket : public PushFYICodec {
protected:
	StreamSocket();

public:
    virtual ~StreamSocket();

    static PushFYICodec* CreateCodec();

    ssize_t encode(const char* src, char* dst);
    int decode(char* src, rxUInt length, char* target, rxUInt& processed);

private:

    int decode_hybi(unsigned char* src, rxUInt srclength, char* target, rxUInt& processed);
    int encode_hybi(const char* src, char* dst);
};

#endif //__STREAMSOCKET_PROTOCOL__
