#include "ts/TSParser.h"

#include <gmock/gmock.h>

#include "TestUtils.h"

TEST(TSParser, ShouldConstructInstance) {
	ts::TSParser parser;
	UNUSED(parser);
}

TEST(TSParser, ShouldSetCallbackPacket) {
	ts::TSParser parser;
	parser.setCallbackPacket([](const ts::TSPacket& packet) {});
}

