#pragma once

#include <functional>
#include <vector>

#include "ts/Packet.h"

namespace ts {

	/// @class Parser
	/// @brief Parse a byte stream into packets
	class Parser {
	public:
		Parser();
		~Parser();

		typedef std::function<void(const Packet& packet)> CallbackPacket;

		void setCallbackPacket(CallbackPacket callback);

		void parse(const uint8_t* buffer, size_t length);

	private:
		void emitPacketFromBuffer(const uint8_t* buffer);

		CallbackPacket m_callbackPacket;

		std::vector<uint8_t> m_cacheBuffer;

		uint8_t packetCache[ts::Packet::kLength];
		size_t packetCacheUsed;
	};

}