#pragma once

#include "CaptureFilter.h"

#include "DirectShowUtils.h"

#include <Tuner.h>

namespace ds {
	class DVBTuner {
	public:
		DVBTuner();
		~DVBTuner();

		static bool init();
		static bool shutdown();

		bool createGraph();

		bool setCallbackTransportStream(CallbackTransportStream callback);

		/// @param frequency - specified in kHz
		bool tuneToFrequency(long frequency);

		bool start();

		bool stop();

	private:
		CComPtr<IGraphBuilder> m_graph;
		CComPtr<IMediaControl> m_mediaControl;
		CComPtr<IMediaEvent> m_mediaEvent;
		CComPtr<ITuningSpace> m_tuningSpace;
		CComPtr<ITuner> m_networkProviderTuner;
		CComPtr<CaptureFilter> mcaptureFilter;
	};
}
