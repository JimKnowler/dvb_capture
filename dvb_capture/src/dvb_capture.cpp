// dvb_capture.cpp : Defines the entry point for the console application.
//

#include "ds/stdafx.h"
#include "ds/DVBTuner.h"
#include "util/FileSink.h"

#include "ts/Parser.h"

#include <assert.h>
#include <memory>

const long kTuneFrequency = 490 * 1000;		// kHz

void run();

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

	dvbTuner->setCallbackTransportStream([&](const BYTE* buffer, long length) {		
		printf("Transport stream data -> [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);

		fileSink.write(buffer, length);

		parser.parse(buffer, length);
	});
	
	printf("Tune\n");
	dvbTuner->tuneToFrequency(kTuneFrequency);

	printf("start the graph\n");
	dvbTuner->start();

	printf("wait 3 seconds\n");
	Sleep(3 * 1000);

	printf("stop the graph\n");
	dvbTuner->stop();

	printf("\nSUCCESSFULLY finished execution\n\n");
}

