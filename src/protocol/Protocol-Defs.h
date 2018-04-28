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

#ifndef __PROTOCOL_DEFS__
#define __PROTOCOL_DEFS__

#define MAX_BUFFER_SIZE		10*1024
#define MAX_PAYLOAD_SIZE	64*1024

#define WEBSOCKET_PROTOCOL	"websocket"
#define STREAMSOCKET_PROTOCOL 	"stream"

#define POLICY_RESPONSE "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\n"

#define HYBI_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define HYBI10_ACCEPTHDRLEN 29

#define SERVER_HANDSHAKE_HYBI "HTTP/1.1 101 Switching Protocols\r\n\
Upgrade: websocket\r\n\
Connection: Upgrade\r\n\
Origin: http:\/\/192.168.1.14\r\n\
Host: 192.168.0.4:8082\r\n\
Sec-WebSocket-Accept: %s\r\n\
Sec-WebSocket-Protocol: %s\r\n\
\r\n"

#define HTTP_POST_EVENT_SOURCE_RESPONSE	"HTTP/1.1 200 OK\r\n\
Content-Type: application/json\r\n\
Content-Length: %d\r\n\
\r\n\
%s"

#define PUSHFYI_REALTIME_EVENT	"pushfyi \r\n\
Content-Length: %d\r\n\
\r\n\
%s"

/*!
 * Security Types
 *
 * Security			Code
 * ------------------------------------
 * Plain Text		0x100000
 * SSL				0x010000
 * TLS				0x110000
 */
typedef enum __pushfyi_security {
	ePushfyiUnknownSecurity = 0x000000,
	ePushfyiPlainText = 0x100000,
	ePushfyiTLS = 0x010000,
	ePushfyiUnsupportedSecurity = 0x110000
} PUSHFYI_SECURITY;

/*!
 * List of protocols supported
 *
 * Protocol			Code
 * ------------------------------------
 * Hixie75			0x000100
 * Hixie76			0x000101
 * HyBi				0x000110
 */
typedef enum __pushfyi_protocol {
	eProtocolUnknown = 0x000001,
	eProtocolInvalidHeader = 0x000010,
	eProtocolNotSupported = 0x000011,
	eProtocolWebSocket = 0x000100,
	eProtocolStreamSocket = 0x000101,
	eProtocolEventSource = 0x000110,
	eTerminated = 0x000111
} PUSHFYI_PROTOCOL;

/*!
 *
 * PUSH FYI REQUEST HEADER
 */
typedef struct __pushfyi_header {
	char path[1024+1];
	char host[1024+1];
	char agent[1024+1];
	char origin[1024+1];
	char version[1024+1];
	char connection[1024+1];
	char content[1024+1];
	char length[1024+1];
	char protocols[1024+1];
	char key1[1024+1];
	char key2[1024+1];
	char key3[8+1];
} PUSHFYI_HEADER;

typedef PUSHFYI_HEADER* P_PUSHFYI_HEADER;

#define EVENT_SOURCE_CONTENT_TYPE_XML	"application/xml"
#define EVENT_SOURCE_CONTENT_TYPE_FORM	"application/x-www-form-urlencoded"

#endif //__PROTOCOL_DEFS__
