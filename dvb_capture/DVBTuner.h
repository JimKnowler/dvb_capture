#pragma once

#include "DirectShowUtils.h"

#include <Tuner.h>

class DVBTuner {
public:
	DVBTuner();
	~DVBTuner();

	static bool init();
	static bool shutdown();

	bool createGraph();

	/// @param frequency - specified in kHz
	bool tuneToFrequency(long frequency);

	bool start();

	bool stop();

private:
	CComPtr<IGraphBuilder> graph;
	CComPtr<IMediaControl> mediaControl;
	CComPtr<IMediaEvent> mediaEvent;
	CComPtr<ITuningSpace> tuningSpace;
	CComPtr<ITuner> networkProviderTuner;
};
