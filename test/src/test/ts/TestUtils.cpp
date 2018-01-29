#include "TestUtils.h"

#include <fstream>

#include "ts/Parser.h"

using std::ios;

std::streampos helperGetFileSize(const char* filename) {
	std::ifstream file(filename, ios::in | ios::binary);
	
	std::streampos fileBegin = file.tellg();
	
	file.seekg(0, std::ios::end);	
	std::streampos fileEnd = file.tellg();
	
	file.close();

	std::streampos fileSize = fileEnd - fileBegin;

	return fileSize;
}

std::vector<uint8_t> helperReadFile(const char* filename) {
	std::streampos fileSize = helperGetFileSize(filename);
	
	std::ifstream file;
	file.open(filename, ios::in | ios::binary);

	std::vector<uint8_t> buffer(fileSize);
	file.read(reinterpret_cast<char*>(&buffer.front()), fileSize);

	return buffer;	
}

int helperCountTSPacketsInFile(const char* filename, int chunkSize, int inNumBytes) {
	int numPackets = 0;

	ts::Parser parser;
	parser.setCallbackPacket([&](const ts::Packet& packet) {
		numPackets += 1;
	});
	
	std::vector<uint8_t> buffer = helperReadFile(filename);

	size_t numBytes;
	if (inNumBytes == -1) {
		numBytes = buffer.size();
	} else {
		numBytes = size_t(inNumBytes);
	}

	if (chunkSize == -1) {
		// parse in a single buffer
		parser.parse(&buffer.front(), numBytes);
	} else {
		// parse as a sequence of smaller chunks
		size_t readIndex = 0;
		size_t end = numBytes - chunkSize;
		const uint8_t* pBuffer = &buffer.front();
		while (readIndex <= end) {
			parser.parse(pBuffer+readIndex, chunkSize);
			readIndex += chunkSize;
		}
	}

	return numPackets;
}
