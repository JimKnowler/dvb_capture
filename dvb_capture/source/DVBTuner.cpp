#include "stdafx.h"
#include <assert.h>

#include "DVBTuner.h"

//#include <Bdaiface.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdamedia.h>



// common HRESULT values 
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa378137(v=vs.85).aspx
// 0x80004002 - No such interface supported
// 0x80004005 - Unspecified failure
// 0x80070057 - One or more arguments are not valid

// DirectShow HRESULT values
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375623(v=vs.85).aspx
// 0x80040217 - No combination of intermediate filters could be found to make the connection.
// 0x80040251 -	No media time was set for this sample

DVBTuner::DVBTuner() {

}

DVBTuner::~DVBTuner() {

}

bool DVBTuner::init() {
	// Initialize COM library
	VALIDATE_HR(CoInitialize(NULL));

	return true;
}

bool DVBTuner::shutdown() {
	CoUninitialize();

	return true;
}

bool DVBTuner::createGraph() {
	// Enumerate all filters with category KSCATEGORY_BDA_NETWORK_TUNER.
	// These filters are usually installed by vendors of hardware tuners
	// when they provide BDA-compatible drivers.
	std::vector<CComPtr<::IMoniker>> tunerMonikers;
	VALIDATE_HR(enumerateDevicesByClass(KSCATEGORY_BDA_NETWORK_TUNER, tunerMonikers));
	assert(tunerMonikers.size() == 1);

	CComPtr<IMoniker> moniker;
	moniker = tunerMonikers[0];

	// Create instance of Filter Graph Manager	
	VALIDATE_HR(CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_graph));

	// get control interfaces for the Filter Graph Manager
	VALIDATE_HR(m_graph->QueryInterface(IID_IMediaControl, (void **)&m_mediaControl));
	VALIDATE_HR(m_graph->QueryInterface(IID_IMediaEvent, (void **)&m_mediaEvent));

	// create network provider
	CComPtr<IBDA_NetworkProvider> networkProvider;
	VALIDATE_HR(CoCreateInstance(CLSID_NetworkProvider, NULL,
		CLSCTX_INPROC_SERVER, __uuidof(IBDA_NetworkProvider), (void**)&networkProvider));

	// get IBaseFilter interface for network provider
	CComPtr<IBaseFilter> networkProviderBaseFilter;
	VALIDATE_HR(networkProvider->QueryInterface(IID_IBaseFilter, (void **)&networkProviderBaseFilter));

	// get ITuner interface for network provider	
	VALIDATE_HR(networkProvider->QueryInterface(__uuidof(ITuner), (void **)&m_networkProviderTuner));

	// create instance of tuner from moniker
	CComPtr<IBaseFilter> tunerBaseFilter;
	VALIDATE_HR(moniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&tunerBaseFilter));

	// add network provider to the graph
	VALIDATE_HR(m_graph->AddFilter(networkProviderBaseFilter, L"NetworkProvider"));
	VALIDATE_HR(m_graph->AddFilter(tunerBaseFilter, L"Tuner"));

	// connect Network Provider to Tuner
	VALIDATE_HR(connectFilters(m_graph, networkProviderBaseFilter, tunerBaseFilter));

	// enumerate Tuning Spaces
	CComPtr<ITuningSpaceContainer> tuningSpaceContainer;
	VALIDATE_HR(CoCreateInstance(__uuidof(SystemTuningSpaces), NULL,
		CLSCTX_INPROC_SERVER, __uuidof(ITuningSpaceContainer), (void**)&tuningSpaceContainer));

	CComPtr<IEnumTuningSpaces> enumTuningSpaces;
	VALIDATE_HR(tuningSpaceContainer->get_EnumTuningSpaces(&enumTuningSpaces));

	/// @note this assumes only one tuning space	
	while (!m_tuningSpace.p) {
		ULONG numFetched = 0;
		VALIDATE_HR(enumTuningSpaces->Next(1, &m_tuningSpace, &numFetched));

		assert(numFetched > 0);

		BSTR friendlyName;
		VALIDATE_HR(m_tuningSpace->get_FriendlyName(&friendlyName));
		printf("FOUND TUNING SPACE: %S\n", friendlyName);

		if (FAILED(INVOKE_HR(m_networkProviderTuner->put_TuningSpace(m_tuningSpace)))) {
			m_tuningSpace.Release();
		}
		else {
			printf("CHOSEN TUNING SPACE: %S\n", friendlyName);
		}

		SysFreeString(friendlyName);
	}

	// Enumerate Receivers
	std::vector<CComPtr<IMoniker>> receiverMonikers;
	VALIDATE_HR(enumerateDevicesByClass(KSCATEGORY_BDA_RECEIVER_COMPONENT, receiverMonikers));
	assert(1 == receiverMonikers.size());
	CComPtr<IMoniker> receiverMoniker = receiverMonikers[0];

	// Create Receiver from moniker	
	CComPtr<IBaseFilter> baseFilterReceiver;
	VALIDATE_HR(receiverMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&baseFilterReceiver));

	// Add Receiver to Graph
	VALIDATE_HR(m_graph->AddFilter(baseFilterReceiver, L"Receiver"));
	VALIDATE_HR(connectFilters(m_graph, tunerBaseFilter, baseFilterReceiver));

	// Add Infinite Tee filter (to split output)
	CComPtr<IBaseFilter> baseFilterTee;
	VALIDATE_HR(CoCreateInstance(CLSID_InfTee, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&baseFilterTee));
	VALIDATE_HR(m_graph->AddFilter(baseFilterTee, L"Tee"));
	VALIDATE_HR(connectFilters(m_graph, baseFilterReceiver, baseFilterTee));

	// Add CaptureFilter (where we can receive the transport stream)
	mcaptureFilter = new CaptureFilter();

	CComPtr<IBaseFilter> baseFilterCaptureFilter;
	VALIDATE_HR(mcaptureFilter->QueryInterface(__uuidof(IBaseFilter), (void**)&baseFilterCaptureFilter));

	VALIDATE_HR(m_graph->AddFilter(baseFilterCaptureFilter, L"CaptureFilter"));
	VALIDATE_HR(connectFilters(m_graph, baseFilterTee, baseFilterCaptureFilter));


	// add MPEG-2 De-Multiplexer (so that graph can run)
	CComPtr<IBaseFilter> baseFilterDemux;
	VALIDATE_HR(CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&baseFilterDemux));

	VALIDATE_HR(m_graph->AddFilter(baseFilterDemux, L"MPEG2-Demux (vestigial)"));
	VALIDATE_HR(connectFilters(m_graph, baseFilterTee, baseFilterDemux));

	// enumerate Transport Information Filters (TIF)
	std::vector<CComPtr<IMoniker>> transportInformationMonikers;
	VALIDATE_HR(enumerateDevicesByClass(KSCATEGORY_BDA_TRANSPORT_INFORMATION, transportInformationMonikers));

	for (auto transportInformationMoniker : transportInformationMonikers) {
		CComPtr<IBaseFilter> baseFilterTransportInformation;
		VALIDATE_HR(transportInformationMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&baseFilterTransportInformation));

		VALIDATE_HR(m_graph->AddFilter(baseFilterTransportInformation, L"TIF"));

		if (SUCCEEDED(connectFilters(m_graph, baseFilterDemux, baseFilterTransportInformation))) {
			printf("TRANSPORT INFORMATION FILTER CONNECTED\n");
			break;
		}
		else
		{
			VALIDATE_HR(m_graph->RemoveFilter(baseFilterTransportInformation));
		}
	}
	
	return true;
}

bool DVBTuner::setCallbackTransportStream(CallbackTransportStream callback) {

	return mcaptureFilter->setCallbackTransportStream(callback);
}

bool DVBTuner::tuneToFrequency(long frequency)
{
	CComPtr<ITuneRequest> tuneRequest;
	VALIDATE_HR(m_tuningSpace->CreateTuneRequest(&tuneRequest));

	CComPtr<IDVBTuneRequest> dvbTuneRequest;
	VALIDATE_HR(tuneRequest->QueryInterface(__uuidof(IDVBTuneRequest), (void**)&dvbTuneRequest));

	VALIDATE_HR(dvbTuneRequest->put_ONID(-1));
	VALIDATE_HR(dvbTuneRequest->put_TSID(-1));
	VALIDATE_HR(dvbTuneRequest->put_SID(-1));


	CComPtr<IDVBTLocator> dvbtLocator;
	VALIDATE_HR(CoCreateInstance(__uuidof(DVBTLocator), NULL,
		CLSCTX_INPROC_SERVER, __uuidof(IDVBTLocator), (void**)&dvbtLocator));

	VALIDATE_HR(dvbtLocator->put_CarrierFrequency(frequency));		// @note specified in kHz
	VALIDATE_HR(dvbtLocator->put_Modulation(BDA_MOD_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_Bandwidth(8));
	VALIDATE_HR(dvbtLocator->put_Guard(BDA_GUARD_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_LPInnerFEC(BDA_FEC_VITERBI));
	VALIDATE_HR(dvbtLocator->put_LPInnerFECRate(BDA_BCC_RATE_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_Mode(BDA_XMIT_MODE_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_HAlpha(BDA_HALPHA_NOT_DEFINED));


	CComPtr<IDigitalLocator> digitalLocator = dvbtLocator;

	VALIDATE_HR(tuneRequest->put_Locator(digitalLocator));
	VALIDATE_HR(m_networkProviderTuner->put_TuneRequest(tuneRequest));

	return true;
}

bool DVBTuner::start() {
	VALIDATE_HR(m_mediaControl->Run());

	return true;
}

bool DVBTuner::stop() {
	VALIDATE_HR(m_mediaControl->Stop());

	return true;
}
