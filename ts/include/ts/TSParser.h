#pragma once

#include <functional>

#include "ts/TSPacket.h"

namespace ts {

	class TSParser {
	public:
		
		typedef std::function<void(const TSPacket& packet)> CallbackPacket;

		void setCallbackPacket(CallbackPacket callback);
	};

}