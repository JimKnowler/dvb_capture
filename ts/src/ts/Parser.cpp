#include "ts/Parser.h"
#include "ts/Packet.h"

#include <algorithm>

namespace ts {

	Parser::CallbackPacket nullCallback = [](const Packet&){};

	Parser::Parser() : m_callbackPacket(nullCallback), packetCacheUsed(0)
	{

	}

	Parser::~Parser()
	{

	}

	void Parser::setCallbackPacket(CallbackPacket inCallback) {
		m_callbackPacket = inCallback;
	}

	void Parser::parse(const uint8_t* buffer, size_t length)
	{
		size_t i = 0;

		if (packetCacheUsed > 0) {
			// append bytes to the end of the packet cache buffer
			size_t numBytesRequired = ts::Packet::kLength - packetCacheUsed;
			size_t numBytes = std::min(length, numBytesRequired);
			memcpy(packetCache + packetCacheUsed, buffer, numBytes);
			packetCacheUsed += numBytes;
			i += numBytes;

			// emit packet if we have enough data
			if (packetCacheUsed == ts::Packet::kLength) {
				emitPacketFromBuffer(packetCache);
				packetCacheUsed = 0;
			}
		}
	
		if (length >= ts::Packet::kLength) {
			// emit whole packets in this buffer

			size_t end = length - ts::Packet::kLength;

			while (i <= end) {
				if (buffer[i] == ts::Packet::kSyncByte) {

					emitPacketFromBuffer(buffer + i);

					i += ts::Packet::kLength;
				}
				else {
					i += 1;
				}
			}
		}

		/// cache partial packets from the the end of this buffer
		size_t numBytesRemaining = length - i;
		while ((numBytesRemaining > 0) && (buffer[i] != ts::Packet::kSyncByte)) {
			// search for start of packet in remaining data
			i++;
			numBytesRemaining -= 1;
		}

		if (numBytesRemaining > 0) {			
			memcpy(packetCache, buffer + i, numBytesRemaining);
			packetCacheUsed = numBytesRemaining;
		}
	}

	void Parser::emitPacketFromBuffer(const uint8_t* buffer) {
		static int emitCount = 0;
		emitCount += 1;

		const ts::Packet* packet = reinterpret_cast<const ts::Packet*>(buffer);
		m_callbackPacket(*packet);
	}

}