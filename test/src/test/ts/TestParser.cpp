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

TEST(Parser, ShouldParseDVBCapture1InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_1.dat");
	EXPECT_EQ(47963, numPackets);
}

TEST(Parser, ShouldParseDVBCapture2InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_2.dat");
	EXPECT_EQ(47962, numPackets);
}

TEST(Parser, ShouldParseDVBCapture3InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_3.dat");
	EXPECT_EQ(47963, numPackets);
}

