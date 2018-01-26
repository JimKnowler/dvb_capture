#pragma once

#include <functional>

#include "ts/Packet.h"

namespace ts {

	class Parser {
	public:
		
		typedef std::function<void(const Packet& packet)> CallbackPacket;

		void setCallbackPacket(CallbackPacket callback);
	};

}