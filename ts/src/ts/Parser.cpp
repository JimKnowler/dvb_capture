#include "ts/Parser.h"
#include "ts/Packet.h"

#include <algorithm>
#include <assert.h>

template<class T>
T swapByteOrder(const T& inValue) {
	T value;
	uint8_t* byteValue = reinterpret_cast<uint8_t*>(&value);
	const uint8_t* byteInValue = reinterpret_cast<const uint8_t*>(&inValue);

	const int kSize = sizeof(T);
	const int kHalfSize = kSize / 2;

	for (int i = 0; i < kHalfSize; i++) {
		int swapIndex = kSize - (i + 1);
		byteValue[i] = byteInValue[swapIndex];
		byteValue[swapIndex] = byteInValue[i];
	}

	return value;
}

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

	void Parser::emitPacketFromBuffer(const uint8_t* inBuffer) {		
		uint8_t buffer[ts::Packet::kLength];

		// convert Network BIG Endian to Intel LITTLE Endian

		const uint32_t uHeaderIn = *(reinterpret_cast<const uint32_t*>(inBuffer));
		uint32_t uHeader = swapByteOrder(uHeaderIn);

		ts::Packet::Header header;
		header.syncByte = (uHeader >> 24) & 0xff;
		header.transportErrorIndicator = (uHeader >> 23) & 1;
		header.payloadUnitStartIndicator = (uHeader >> 22) & 1;
		header.transportPriority = (uHeader >> 21) & 1;
		header.pid = (uHeader >> 8) & 0x1fff;
		header.transportScramblingControl = ts::Packet::TransportScramblingControl((uHeader >> 6) & 3);
		header.adaptationFieldControl = ts::Packet::AdaptationFieldControl((uHeader >> 4) & 3);
		header.continuityCounter = uHeader & 0xf;		

		memcpy(buffer, &header, 4);
		memcpy(buffer + 4, inBuffer + 4, ts::Packet::kLength - 4);
				
		const ts::Packet* packet = reinterpret_cast<const ts::Packet*>(buffer);
		assert(packet->header.syncByte == Packet::kSyncByte);


		m_callbackPacket(*packet);
	}

}