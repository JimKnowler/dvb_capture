// dvb_capture.cpp : Defines the entry point for the console application.
//

#include "ds/stdafx.h"
#include "ds/DVBTuner.h"
#include "util/FileSink.h"

#include "ts/Parser.h"

#include <assert.h>
#include <memory>
#include <map>

const long kTuneFrequency = 490 * 1000;		// kHz

void run();
void onPacket(const ts::Packet& packet);
void reportPacketStats();

int _tmain(int argc, _TCHAR* argv[])
{
	ds::DVBTuner::init();
	
	run();	

	ds::DVBTuner::shutdown();

	return 0;
}

void run() {

	std::unique_ptr<ds::DVBTuner> dvbTuner(new ds::DVBTuner());

	dvbTuner->createGraph();

	util::FileSink fileSink("dvb_capture.dat");
	ts::Parser parser;
	parser.setCallbackPacket([](const ts::Packet& packet) {
		onPacket(packet);
	});

	dvbTuner->setCallbackTransportStream([&](const BYTE* buffer, long length) {				
		fileSink.write(buffer, length);
		parser.parse(buffer, length);
	});
	
	printf("Tune\n");
	dvbTuner->tuneToFrequency(kTuneFrequency);

	printf("start the graph\n");
	dvbTuner->start();

	printf("wait 10 seconds\n");
	Sleep(10 * 1000);

	printf("stop the graph\n");
	dvbTuner->stop();

	printf("\nSUCCESSFULLY finished execution\n\n");

	reportPacketStats();
}

namespace {
	std::map<uint32_t, int> pidMap;
}

void onPacket(const ts::Packet& packet) {
	uint32_t pid = packet.header.pid;
	if (pidMap.find(pid) == pidMap.end()) {
		printf("found PID [%u]\n", pid);
		pidMap[pid] = 0;
	}
	
	pidMap[pid] += 1;
	
}

void reportPacketStats() {
	printf("\n\nPACKET STATS\n");
	auto it = pidMap.begin();
	for (; it != pidMap.end(); it++) {
		uint32_t pid = it->first;
		uint32_t packetCount = it->second;
		printf(" >> PID [%04u] => [%d]\n", pid, packetCount);
	}
}