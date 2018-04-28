/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Protocol Helper Functions

*********************************************************
*/

#ifndef __PUSHFYI_HELPER__
#define __PUSHFYI_HELPER__

#include "Protocol-Defs.h"

class ProtocolHelper {
public:
	/*
	 * Detect The Security Scheme of The Client
	 * By Parsing the Handshake.
	 *
	 * We Support SSL 3.1
	 * Rest all schemes must be discarded including plain text
	 */
	static PUSHFYI_SECURITY DetectSecurityScheme(char* handshake);

	static void gen_sha1(P_PUSHFYI_HEADER pHeader, char *target);
};

#endif //__PUSHFYI_HELPER__
