#include "ts/Parser.h"

#include <gmock/gmock.h>

#include "TestUtils.h"

TEST(Parser, ShouldConstructInstance) {
	ts::Parser parser;
	UNUSED(parser);
}

TEST(Parser, ShouldSetCallbackPacket) {
	ts::Parser parser;
	parser.setCallbackPacket([](const ts::Packet& packet) {});
}

