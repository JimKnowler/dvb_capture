#pragma once

#include <functional>

#include "ts/Packet.h"

namespace ts {

	class Parser {
	public:
		
		typedef std::function<void(const Packet& packet)> CallbackPacket;

		void setCallbackPacket(CallbackPacket callback);

		void parse(uint8_t* buffer, size_t length);

	private:
		CallbackPacket callbackPacket;
	};

}