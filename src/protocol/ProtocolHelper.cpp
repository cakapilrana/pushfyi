/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: Protocol Type Definitions
*********************************************************
*/

#include <string.h>
#include <sys/types.h>
#include <resolv.h>
#include <assert.h>
#include "tls.h"
#include "Log.h"
#include "ProtocolHelper.h"

PUSHFYI_SECURITY ProtocolHelper::DetectSecurityScheme(char* handshake)
{
    //We accept only SSLv3 / TLSv1 security schemes
    if((bcmp(handshake, "\x16", 1) == 0) ||
               (bcmp(handshake, "\x80", 1) == 0)) {

        if(
        	((bcmp(handshake + 1, "\x03", 1) == 0) && (bcmp(handshake + 2, "\x01", 1) == 0)) ||
        	((bcmp(handshake + 1, "\x03", 1) == 0) && (bcmp(handshake + 2, "\x02", 1) == 0)) ||
        	((bcmp(handshake + 1, "\x03", 1) == 0) && (bcmp(handshake + 2, "\x03", 1) == 0))
        ) {

        	INFO(Log::eProtocol, "SSL/ TLS v1 (%d:%d) connection requested", *(handshake+1), *(handshake+2));
            return ePushfyiTLS;
        } else {
        	INFO(Log::eProtocol, "Unsupported security scheme (%d.%d) requested.", *(handshake+1), *(handshake+2));
        	//return eProtocolNotSupported;
        	return ePushfyiUnsupportedSecurity;
        }
    } else {
        INFO(Log::eProtocol, "Plain text connection requested.");
        //_security = ePushfyiPlainText;
        return ePushfyiPlainText;
    }
}

void ProtocolHelper::gen_sha1(P_PUSHFYI_HEADER pHeader, char *target) {
    SHA_CTX c;
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA1_Init(&c);
    SHA1_Update(&c, pHeader->key1, strlen(pHeader->key1));
    SHA1_Update(&c, HYBI_GUID, 36);
    SHA1_Final(hash, &c);

    int r = b64_ntop(hash, sizeof hash, target, HYBI10_ACCEPTHDRLEN);
    assert(r == HYBI10_ACCEPTHDRLEN - 1);
}
