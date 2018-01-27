#include "ts/Parser.h"
#include "ts/Packet.h"

namespace ts {
	void Parser::setCallbackPacket(CallbackPacket inCallback) {
		callbackPacket = inCallback;
	}

	void Parser::parse(uint8_t* buffer, size_t length)
	{
		size_t i = 0;
		size_t end = length - ts::Packet::kSyncByte;

		while (i <= end ) {
			if (buffer[i] == ts::Packet::kSyncByte) {

				const ts::Packet* packet = reinterpret_cast<const ts::Packet*>(buffer + i);
				callbackPacket(*packet);

				i += ts::Packet::kLength;
			}
			else {
				i += 1;
			}
		}

		/// @todo handle left-over data from previous parse requests

		/// @todo save left-over data from this parse request
	}

}