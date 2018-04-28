#include <arpa/inet.h>
#include <string.h>
#include <byteswap.h>
#include "StreamSocket.h"
#include "Log.h"

using namespace std;

StreamSocket::StreamSocket()
{
}

PushFYICodec* StreamSocket::CreateCodec()
{
	return new StreamSocket();
}

StreamSocket::~StreamSocket()
{
}

ssize_t StreamSocket::encode(const char* src, char* dst)
{
	if(strlen(src) <= 0)
		return 0;

	/*
	 * Lets encode
	 */
	return encode_hybi(src, dst);
	//INFO(Log::eProtocol, "Encoded bytes = %s", dst);
}

int StreamSocket::encode_hybi(const char* src, char* target)
{
	INFO(Log::eNetwork, "Encoding text frame");

	rxUInt length = strlen(src);
	sprintf(target, PUSHFYI_REALTIME_EVENT, length, src);

    return strlen(target);
}

int StreamSocket::decode(char* src, rxUInt length, char* target, rxUInt& processed)
{
	return decode_hybi((unsigned char*)src, length, target, processed);
}

int StreamSocket::decode_hybi(unsigned char* src, rxUInt srclength, char* target, rxUInt& processed)
{
	/**
	 * We need to attempt to decode source buffer.
	 * There can be a lot of frames in the source
	 * buffer.
	 *
	 * Decode them all.
	 *
	 * This method will return the opcode.
	 *
	 * else -1 to indicate an error.
	 *
	 * return
	 *
	 * -1 decode complete
	 * -2 truncated frame
	 * -3 terminate
	 */
	char* frame = (char*)src;
	rxInt opcode = 0;

	rxUInt headerLength = 0; rxUInt length = 0;
	char* start, *end;

	if (frame > (char*)src + srclength) {
		ERROR(Log::eProtocol, "Truncated frame from client, need %d more bytes.", frame - ((char*)src + srclength) );
		return -2;
	}

	size_t remaining = ((char*)src + srclength) - frame;
	if(remaining<2)
	{
		INFO(Log::eProtocol, "Frames decoded completely.");
		return -1;
	}

	/*
	 * Make sure this is a PushFYI message
	 */
	bool isPushFYIMessage = false;
	start = strstr(frame, "\r\n\r\n");
	if (start) {
		isPushFYIMessage = true;

		INFO(Log::eProtocol, "Stream protocol payload detected");
		/*
		 * get the header length
		 */
		headerLength = start - frame + 4;
	} else {
		INFO(Log::eProtocol, "Doesn't seem like a PushFYI Message Request");
		return -2;
	}

	if(isPushFYIMessage) {

		char buffer[1024] = {0};

		start = strcasestr(frame, "\r\nContent-Length: ");
		if(!start)
			return -3;

		start += 18;
		end = strstr(start, "\r\n");
		strncpy(buffer, start, end-start);
		buffer[end-start] = '\0';

		length = atoi(buffer);

		INFO(Log::eProtocol, "Payload length = %d", length);

		/*
		 * Check if we have enough data
		 */
		if(srclength < length + headerLength) {
			INFO(Log::eProtocol, "Not enough data read");
			return -2;
		}

		start = strstr(frame, "\r\n\r\n");
		strncpy(target, start + 4, length);

		opcode = 1;
	}

	processed += headerLength + length;

	return opcode;
}
