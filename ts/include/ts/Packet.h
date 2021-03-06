#pragma once

#include <stdint.h>

namespace ts {
	
	class Packet {
	public:

		enum AdaptationFieldControl : uint32_t {
			kReserved = 0,
			kPayloadOnly = 1,
			kAdaptationFieldOnly = 2,
			kAdaptationFieldFollowedByPayload = 3,			
		};
		
		enum TransportScramblingControl : uint32_t {
			kNotScrambled = 0,
			kReservedForFutureUse = 1,
			kScrambledWithEvenKey = 2,
			kScrambledWithOddKey = 3
		};

		/// @struct Header
		/// @note Remember to convert Big Endian network order to Local Endian
		struct Header {
			uint32_t syncByte : 8;
			uint32_t transportErrorIndicator : 1;
			uint32_t payloadUnitStartIndicator : 1;
			uint32_t transportPriority : 1;
			uint32_t pid : 13;
			TransportScramblingControl transportScramblingControl : 2;
			AdaptationFieldControl adaptationFieldControl : 2;
			uint32_t continuityCounter : 4;
		};

		/// @struct AdaptationFieldHeader
		/// @note Remember to convert Big Endian network order to Local Endian
		struct AdaptationFieldHeader {
			uint16_t adaptationFieldLength : 8;
			uint16_t discontinuityIndicator : 1;
			uint16_t randomAccessIndicator : 1;
			uint16_t elementaryStreamPriorityIndicator : 1;
			uint16_t flagPCR : 1;
			uint16_t flagOPCR : 1;
			uint16_t flagSplicingPoint : 1;
			uint16_t flagTransportPrivateData : 1;
			uint16_t flagAdaptationFieldExtension : 1;
		};

		struct PCR {
			/// @todo 33 bits base
			/// 6 bits reserved
			/// 9 bits extension
			uint8_t pcr[6];
		};

		struct OPCR : PCR {

		};

		struct SpliceCountdown {
			int8_t spliceCountdown;
		};

		struct TransportPrivateDataLength {
			uint8_t transportPrivateDataLength;
		};

		/// @struct AdaptatoinExtensionField
		/// @note Remember to convert Big Endian network order to Local Endian
		struct AdaptationExtensionField {
			uint16_t adaptationExtensionLength : 8;
			uint16_t flagLegalTimeWindow : 1;
			uint16_t flagPiecewiseRate : 1;
			uint16_t flagSeamlessSpliceFlag : 1;
			uint16_t reserved : 5;
		};

		/// @struct LegalTimeWindow
		/// @note Remember to convert Big Endian network order to Local Endian
		struct LegalTimeWindow {
			uint16_t flagLegalTimeWindowValid : 1;
			uint16_t legalTimeWindowOffset : 15;
		};

		struct PiecewiseRate {
			uint8_t data[3];

			/// @todo reserved : 2
			/// @todo piecewiseRate : 22
		};

		struct SeamlessSplice {
			uint8_t data[5];

			/// @todo spliceType : 5
			/// @todo nextAccessUnitDTS : 36
		};

		enum : uint8_t {
			kStuffingByte = 0xff,
			kSyncByte = 0x47,
			kLength = 188
		};

		Header header;

		uint8_t data[184];
	};
}