#include "ts/Packet.h"

#include <gmock/gmock.h>
#include "TestUtils.h"

TEST(Packet, ShouldConstruct) {
	ts::Packet packet;
	UNUSED(packet);
}

TEST(Packet, ShouldPackHeaderToCorrectSize) {
	ts::Packet::Header header;
	UNUSED(header);

	unsigned headerSize = sizeof(header);
	EXPECT_EQ(4, headerSize);
}

TEST(Packet, ShouldPackAdaptationFieldHeaderToCorrectSize) {
	ts::Packet::AdaptationFieldHeader adaptationFieldHeader;
	UNUSED(adaptationFieldHeader);

	unsigned adaptationFieldHeaderSize = sizeof(adaptationFieldHeader);
	EXPECT_EQ(2, adaptationFieldHeaderSize);
}

TEST(Packet, ShouldPackOptionalAdaptationFieldToCorrectSizePCR) {
	ts::Packet::PCR pcr;
	UNUSED(pcr);

	unsigned pcrSize = sizeof(pcr);
	EXPECT_EQ(6, pcrSize);
}

TEST(Packet, ShouldPackOptionalAdaptationFieldToCorrectSizeOPCR) {
	ts::Packet::OPCR opcr;
	UNUSED(opcr);

	unsigned opcrSize = sizeof(opcr);
	EXPECT_EQ(6, opcrSize);
}

TEST(Packet, ShouldPackOptionalAdaptationFieldToCorrectSizeSpliceCountdown) {
	ts::Packet::SpliceCountdown spliceCountdown;
	UNUSED(spliceCountdown);

	unsigned spliceCountdownSize = sizeof(spliceCountdown);
	EXPECT_EQ(1, spliceCountdownSize);
}

TEST(Packet, ShouldPackOptionalAdaptationFieldToCorrectSizeTransportPrivateDataLength) {
	ts::Packet::TransportPrivateDataLength transportPrivateDataLength;
	UNUSED(transportPrivateDataLength);

	unsigned transportPrivateDataLengthSize = sizeof(transportPrivateDataLength);
	EXPECT_EQ(1, transportPrivateDataLengthSize);
}

TEST(Packet, ShouldPackAdaptationExtensionFieldToCorrectSize) {
	ts::Packet::AdaptationExtensionField adaptationExtensionField;
	UNUSED(adaptationExtensionField);

	unsigned adaptationExtensionFieldSize = sizeof(adaptationExtensionField);
	EXPECT_EQ(2, adaptationExtensionFieldSize);
}

TEST(Packet, ShouldPackLegalTimeWindowFieldToCorrectSize) {
	ts::Packet::LegalTimeWindow legalTimeWindowField;
	UNUSED(legalTimeWindowField);

	unsigned legalTimeWindowFieldSize = sizeof(legalTimeWindowField);
	EXPECT_EQ(2, legalTimeWindowFieldSize);
}

TEST(Packet, ShouldPackPiecewiseRateFieldToCorrectSize) {
	ts::Packet::PiecewiseRate piecewiseRate;
	UNUSED(piecewiseRate);

	unsigned piecewiseRateSize = sizeof(piecewiseRate);
	EXPECT_EQ(3, piecewiseRateSize);
}

TEST(Packet, ShouldPackSeamlessSpliceToCorrectSize) {
	ts::Packet::SeamlessSplice seamlessSplice;
	UNUSED(seamlessSplice);

	unsigned seamlessSpliceSize = sizeof(seamlessSplice);
	EXPECT_EQ(5, seamlessSpliceSize);
}

TEST(Packet, ShouldPackPacketToCorrectSize) {
	ts::Packet packet;
	UNUSED(packet);

	unsigned packetSize = sizeof(packet);
	EXPECT_EQ(188, packetSize);
}

TEST(Packet, ShouldDefineConstantForStuffingByte) {
	EXPECT_EQ(0xFF, ts::Packet::kStuffingByte);
}

TEST(Packet, ShouldDefineConstantForSyncByte) {
	EXPECT_EQ(0x47, ts::Packet::kSyncByte);
}

