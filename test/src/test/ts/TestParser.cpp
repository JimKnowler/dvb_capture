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

const int kNumPacketsDVBCapture1 = 47962;
const int kNumPacketsDVBCapture2 = 47961;
const int kNumPacketsDVBCapture3 = 47962;

TEST(Parser, ShouldParseDVBCapture1InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_1.dat");
	EXPECT_EQ(kNumPacketsDVBCapture1, numPackets);
}

TEST(Parser, ShouldParseDVBCapture2InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_2.dat");
	EXPECT_EQ(kNumPacketsDVBCapture2, numPackets);
}

TEST(Parser, ShouldParseDVBCapture3InSingleBuffer) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_3.dat");
	EXPECT_EQ(kNumPacketsDVBCapture3, numPackets);
}

TEST(Parser, ShouldParseSinglePacketInSingleByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_1.dat", 1, ts::Packet::kLength);
	EXPECT_EQ(1, numPackets);
}

TEST(Parser, ShouldParseDVBCapture1InSingleByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_1.dat", 1);
	EXPECT_EQ(kNumPacketsDVBCapture1, numPackets);
}

TEST(Parser, ShouldParseDVBCapture2InSingleByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_2.dat", 1);
	EXPECT_EQ(kNumPacketsDVBCapture2, numPackets);
}

TEST(Parser, ShouldParseDVBCapture3InSingleByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_3.dat", 1);
	EXPECT_EQ(kNumPacketsDVBCapture3, numPackets);
}

TEST(Parser, ShouldParseDVBCapture1In100ByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_1.dat", 100);
	EXPECT_EQ(kNumPacketsDVBCapture1, numPackets);
}

TEST(Parser, ShouldParseDVBCapture2In100ByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_2.dat", 100);
	EXPECT_EQ(kNumPacketsDVBCapture2, numPackets);
}

TEST(Parser, ShouldParseDVBCapture3In100ByteChunks) {
	int numPackets = helperCountTSPacketsInFile("data/dvb_capture_3.dat", 100);
	EXPECT_EQ(kNumPacketsDVBCapture3, numPackets);
}

TEST(Parser, ShouldParseWithoutSettingCallback) {
	std::vector<uint8_t> buffer = helperReadFile("data/dvb_capture_1.dat");
	ts::Parser parser;
	parser.parse(&buffer.front(), buffer.size());
}

