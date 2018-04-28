/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Type Definitions
*********************************************************
*/

#ifndef __PUSHFYI_TYPES_H__
#define __PUSHFYI_TYPES_H__

#include <sys/types.h>
#include <string>

typedef u_int8_t  rxByte;
typedef int16_t   rxShort;
typedef u_int16_t rxUShort;
typedef int32_t   rxInt;
typedef u_int32_t rxUInt;
typedef int64_t   rxLong;
typedef u_int64_t rxULong;
typedef float     rxFloat;
typedef double    rxDouble;
typedef size_t    rxSize;
typedef rxInt     rxStatus;

const rxUShort RX_MAX_USHORT = rxUShort(0) - 1;
const rxUInt   RX_MAX_UINT   = rxUInt(0) - 1;
const rxULong  RX_MAX_ULONG  = rxULong(0) - 1;
const rxSize   RX_MAX_SIZE   = rxSize(0) - 1;

extern const std::string  RX_ESTR;
extern const std::string& RX_ESREF;


#endif //__PUSHFYI_TYPES_H__
