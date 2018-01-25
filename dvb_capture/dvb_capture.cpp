// dvb_capture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DVBTuner.h"
#include <assert.h>

#include <memory>

const long kTuneFrequency = 490 * 1000;		// kHz

void run();

int _tmain(int argc, _TCHAR* argv[])
{
	DVBTuner::init();
	
	run();	

	DVBTuner::shutdown();

	return 0;
}

void run() {

	std::unique_ptr<DVBTuner> dvbTuner(new DVBTuner());

	dvbTuner->createGraph();
	
	printf("Tune\n");
	dvbTuner->tuneToFrequency(kTuneFrequency);

	printf("start the graph\n");
	dvbTuner->start();

	printf("wait 10 seconds\n");
	Sleep(10 * 1000);

	printf("stop the graph\n");
	dvbTuner->stop();

	printf("\nSUCCESSFULLY finished execution\n\n");
}

