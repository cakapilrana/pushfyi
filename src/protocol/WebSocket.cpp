#include <arpa/inet.h>
#include <string.h>
#include <byteswap.h>
#include "WebSocket.h"
#include "Log.h"

using namespace std;

WebSocket::WebSocket()
{
}

PushFYICodec* WebSocket::CreateCodec()
{
	return new WebSocket();
}

WebSocket::~WebSocket()
{
}

ssize_t WebSocket::encode(const char* src, char* dst)
{
	if(strlen(src) <= 0)
		return 0;

	/*
	 * Lets encode
	 */
	return encode_hybi(src, dst);
	//INFO(Log::eProtocol, "Encoded bytes = %s", dst);
}

int WebSocket::encode_hybi(const char* src, char* target)
{
	rxUInt srclength = strlen(src);
    rxUInt opcode = 1, payload_offset = 2;

    target[0] = (char)( (opcode & 0x0F) | 0x80);

    if (srclength <= 125) {
        target[1] = (char) srclength;
        payload_offset = 2;
    }
    else if ((srclength > 125) && (srclength < 65536)) {
        target[1] = 0x7E;

        uint16_t len = htons(srclength);
        memcpy(target+2, &len, 2);

        payload_offset = 4;
    }
    else {
        ERROR(Log::eNetwork, "Sending frames larger than 65535 bytes not supported\n");
        return -1;
    }
    strncpy(target+payload_offset, src, srclength);

    return srclength + payload_offset;
}

int WebSocket::decode(char* src, rxUInt length, char* target, rxUInt& processed)
{
	return decode_hybi((unsigned char*)src, length, target, processed);
}

int WebSocket::decode_hybi(unsigned char* src, rxUInt srclength, char* target, rxUInt& processed)
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
	unsigned char* frame = src;
	rxInt opcode = 0, byte = 0;

	rxUInt headerLength = 0; rxUInt length = 0; rxUInt masked = 0;

	if (frame > src + srclength) {
		ERROR(Log::eProtocol, "Truncated frame from client, need %d more bytes.", frame - (src + srclength) );
		return -2;
	}

	size_t remaining = (src + srclength) - frame;
	if(remaining<2)
	{
		INFO(Log::eProtocol, "Frames decoded completely.");
		return -1;
	}

	/*
	 * This is the beginning of next frame.
	 * Check the fin bit to start with.
	 */
	byte = frame[0];

	/*
	 * Either this is an
	 *
	 * 1. Unfragmented message or
	 * 2. Final fragment in the message
	 *
	 * i.e fin-bit = 1
	 */
	if(byte>>7 == 1)
	{
		/*
		 * We have a fin bit set.
		 * check the opcode
		 */
		opcode = frame[0] & 0x0f;

		switch(opcode)
		{
			case 0x0:
			case 0x1:
			{
				/*
				 * This is an Unfragmented text frame.
				 * Let's Decode it.
				 */
				if(opcode == 0x1)
					INFO(Log::eProtocol, "Client sent an Unfragmented TEXT frame, opcode =  %d.", opcode);
				else if(opcode == 0x0)
					INFO(Log::eProtocol, "Client sent final Fragmented TEXT frame, opcode =  %d.", opcode);

				/*
				 * Let's check the payload length
				 * of this frame.
				 */
				byte = frame[1];
				length = byte & 0x7F;

				/*
				 * Lets check the masking bit.
				 */
				masked = (byte & 0x80) >> 7;

				if(length == 0) {
					ERROR(Log::eProtocol, "Client sent an Unfragmented EMPTY TEXT frame.");
					/*
					 * We shoud skip it.
					 */
					headerLength = 2;
					return -3;
				} else if(length < 126) {
					headerLength = 2;
				} else if(length == 126) {
					headerLength = 4;
					length = ((frame[2] << 8) + frame[3]);
				} else {
					headerLength = 10;
					length = 1;
				}

				INFO(Log::eProtocol, "Read length = %d, Payload length = %d", srclength, length);

				/*
				 * Check if we have enough data
				 */
				if(srclength < length + headerLength + 4*masked) {
					INFO(Log::eProtocol, "Not enough data read");
					return -2;
				}

				/*
				 * Ensure that Client to Server frame is masked.
				 */
				if(!masked) {
					/*
					 * RFC 6455 requires client to server data to be masked.
					 */
					ERROR(Log::eProtocol, "Client sent an Unfragmented UNMASKED TEXT frame. Skip it.");
					return -3;
				}

				//go to the payload
				unsigned char* payload = frame + headerLength + 4*masked;

				// unmask the data
				unsigned char* mask = payload - 4;
				for (rxUInt i = 0; i < length; i++) {
					target[i] = payload[i] ^ mask[i%4];
				}

				opcode = 1;
			}
			break;

			case 0x2:
			{
				//binary data is not supported for now.
				ERROR(Log::eProtocol, "Binary frame is not supported.");
			}
			break;

			case 0x8:
			{
				/*
				 * Client has closed this connection.
				 */
				INFO(Log::eProtocol, "Client sent a CLOSE frame.");
			}
			break;

			case 0x9:
			{
				/*
				 * This is a PING.
				 */
				INFO(Log::eProtocol, "Client sent a PING frame.");

				headerLength = 0;

				//check the length
				byte = frame[1];
				length = byte & 0x7F;

				if(length < 126) {
					headerLength = 2;
				} else if(length == 126) {
					headerLength = 4;
					length = ((frame[2] << 8) + frame[3]);
				} else {
					ERROR(Log::eProtocol, "DAMN Such a large PING. Terminate it.");
					length = -1;
				}

				INFO(Log::eProtocol, "Payload length = %d", length);

				/*
				 * Its a ping frame we dont care about if it has masked data.
				 * We just send it back as it is.
				 */
				//go to the payload
				unsigned char* payload = frame + headerLength;

				// Terminate with a null for base64 decode
				unsigned char save_char = payload[length];
				payload[length] = '\0';

				// unmask the data
				for (rxUInt i = 0; i < length; i++) {
					target[i] = payload[i];
				}
				payload[length] = save_char;
			}
			break;

			case 0xA:
			{
				/*
				 * This is a PONG.
				 */
				INFO(Log::eNetwork, "Client sent a PONG frame.");

				headerLength = 0;

				//check the length
				byte = frame[1];
				length = byte & 0x7F;

				if(length < 126) {
					headerLength = 2;
				} else if(length == 126) {
					headerLength = 4;
					length = ((frame[2] << 8) + frame[3]);
				} else {
					ERROR(Log::eNetwork, "DAMN Such a large PONG. Terminate it.");
					length = -1;
				}

				INFO(Log::eProtocol, "Payload length = %d", length);

				/*
				 * Its a ping frame we dont care about if it has masked data.
				 * We just send it back as it is.
				 */
				//go to the payload
				unsigned char* payload = frame + headerLength;

				// Terminate with a null for base64 decode
				//unsigned char save_char = payload[length];
				//payload[length] = '\0';

				// unmask the data
				for (rxUInt i = 0; i < length; i++) {
					target[i] = payload[i];
				}
				//payload[length] = save_char;
			}
			break;

			default:
			{
				/*
				 * This is a PING.
				 */
				ERROR(Log::eNetwork, "WTF is that.");
			}
			break;
		}
	} //fin-bit = 1

	/*
	 * This is a Continuation frame.
	 *
	 * i.e fin-bit = 0
	 */
	else if(byte>>7 == 0) {
		/*
		 * We have a fin bit clear.
		 * check the opcode
		 */
		opcode = frame[0] & 0x0f;

		switch(opcode) {
			case 0x0:
			case 0x1:
			{
				/*
				 * This is an Unfragmented text frame.
				 * Let's Decode it.
				 */
				if(opcode == 0x1)
					INFO(Log::eNetwork, "Client sent a fragmented TEXT frame.");
				else if(opcode == 0x0)
					INFO(Log::eNetwork, "Client sent a Continuation TEXT frame.");

				/*
				 * Let's check the payload length
				 * of this frame.
				 */
				byte = frame[1];
				length = byte & 0x7F;

				/*
				 * Lets check the masking bit.
				 */
				masked = (byte & 0x80) >> 7;

				if(length == 0) {
					ERROR(Log::eNetwork, "Client sent an Unfragmented EMPTY TEXT frame.");
					/*
					 * We shoud skip it.
					 */
					headerLength = 2;
					return -3;
				} else if(length < 126) {
					headerLength = 2;
				} else if(length == 126) {
					headerLength = 4;
					length = ((frame[2] << 8) + frame[3]);
				} else {
					headerLength = 10;
					length = 1;
				}

				INFO(Log::eProtocol, "Read length = %d, Payload length = %d", srclength, length);

				/*
				 * Check if we have enough data
				 */
				if(srclength < length + headerLength + 4*masked) {
					INFO(Log::eProtocol, "Not enough data read");
					return -2;
				}

				/*
				 * Ensure that Client to Server frame is masked.
				 */
				if(!masked) {
					/*
					 * RFC 6455 requires client to server data to be masked.
					 */
					ERROR(Log::eNetwork, "Client sent an Unfragmented UNMASKED TEXT frame. Skip it.");
					return -3;
				}

				//go to the payload
				unsigned char* payload = frame + headerLength + 4*masked;

				// Terminate with a null for base64 decode
				//unsigned char save_char = payload[length];
				//payload[length] = '\0';

				// unmask the data
				unsigned char* mask = payload - 4;
				for (rxUInt i = 0; i < length; i++) {
					target[i] = payload[i] ^ mask[i%4];
				}
				//payload[length] = save_char;

				opcode = 0;
			}
			break;
		}
	} //end fin-bit = 0

	else
		ERROR(Log::eProtocol, "What is this thing..");

	processed += headerLength + 4*masked + length;
	return opcode;
}
