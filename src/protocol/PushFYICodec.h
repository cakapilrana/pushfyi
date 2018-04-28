/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Pushfyi Protocol Codec Abstract Class
*********************************************************
*/
#ifndef __PUSHFYI_CODEC__
#define __PUSHFYI_CODEC__

#include "pushfyi-types.h"

class PushFYICodec
{
public:
	PushFYICodec() {};
    virtual ~PushFYICodec() {};

    /*
     * Frame Encoder Method
     */
    virtual ssize_t encode(	/*Input buffer to encode*/const char* src,
    						/*Output buffer*/char* dst) = 0;

    /*
     * Frame Decoder Method
     */
    virtual int decode( /*Input Buffer*/char* src,
    						/*Length of Input buffer*/rxUInt length,
    						/*Output Buffer*/char* target,
    						/*processed bytes*/ rxUInt& bytesProcessed) = 0;
};
#endif //__PUSHFYI_CODEC__
