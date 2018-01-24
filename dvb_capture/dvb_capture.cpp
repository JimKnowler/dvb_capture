// dvb_capture.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <assert.h>

// DirectShow\BaseClasses
typedef wchar_t *PTCHAR;
#include <streams.h>

#include <Dshow.h>
#include <Bdaiface.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdamedia.h>
#include <atlbase.h>
#include <Tuner.h>
#include <vector>


#define LOG_HR( _str, _hr ) printf("0x%08X : %s\n", _hr, _str)
#define INVOKE_HR( _com ) InvokeHR( _com, #_com )
#define VALIDATE_HR( _com ) { HRESULT _hr = (_com); LOG_HR( #_com, _hr); assert(SUCCEEDED(_hr)); }

HRESULT InvokeHR(HRESULT hr, const char* str) {
	LOG_HR(str, hr);
	return hr;
}

// common HRESULT values 
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa378137(v=vs.85).aspx
// 0x80004002 - No such interface supported
// 0x80004005 - Unspecified failure
// 0x80070057 - One or more arguments are not valid

// DirectShow HRESULT values
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375623(v=vs.85).aspx
// 0x80040217 - No combination of intermediate filters could be found to make the connection.
// 0x80040251 -	No media time was set for this sample

#define APP_GUID(_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const ::GUID _name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

APP_GUID(CLSID_CaptureFilter, 0xfd628dda, 0x002a, 0x11e8, 0xba, 0x89, 0x0e, 0xd5, 0xf8, 0x9f, 0x71, 0x8b);

void run();

int _tmain(int argc, _TCHAR* argv[])
{
	// Initialize COM library
	VALIDATE_HR(CoInitialize(NULL));
	
	run();	

	CoUninitialize();

	return 0;
}

HRESULT EnumerateDevicesByClass(const ::CLSID& clsid, std::vector <CComPtr <::IMoniker>>& monikers)
{
	// Reset content of vector
	monikers.clear();

	// Create a DirectShow System Device Enumerator
	CComPtr<ICreateDevEnum> enumDevices;
	VALIDATE_HR(CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&enumDevices));
	
	// Enumerate devices
	CComPtr<::IEnumMoniker> enumMonikers;
	VALIDATE_HR(enumDevices->CreateClassEnumerator(clsid, &enumMonikers, 0));
	
	// Loop on all enumerated providers.
	while (true)
	{
		CComPtr<::IMoniker> moniker;
		ULONG numFetched = 0;
		VALIDATE_HR(enumMonikers->Next(1, &moniker, &numFetched));
		
		if (numFetched == 1)
		{
			monikers.push_back(moniker);

			/// @todo get the name of the moniker
		}
		else 
		{
			break;
		}	
	}

	return S_OK;
}

HRESULT IsPinConnected(CComPtr<IPin> pin, bool& outResult)
{
	CComPtr<IPin> pinTmp;
	HRESULT hr = pin->ConnectedTo(&pinTmp);

	if (SUCCEEDED(hr))
	{
		outResult = true;
	}
	else if (hr == VFW_E_NOT_CONNECTED)
	{
		// The pin is not connected. This is not an error for our purposes.
		outResult = false;
		hr = S_OK;
	}

	return hr;
}

HRESULT IsPinDirection(CComPtr<IPin> pin, PIN_DIRECTION dir, bool& outResult)
{
	PIN_DIRECTION pinDir;
	VALIDATE_HR(pin->QueryDirection(&pinDir));
	
	outResult = (pinDir == dir);
	
	return S_OK;
}

HRESULT MatchPin(CComPtr<IPin> pin, PIN_DIRECTION direction, bool shouldBeConnected, bool& outResult)
{
	bool isMatch = false;
	bool isConnected = false;

	VALIDATE_HR(IsPinConnected(pin, isConnected));
	
	if (isConnected == shouldBeConnected)
	{
		VALIDATE_HR(IsPinDirection(pin, direction, isMatch));
	}

	outResult = isMatch;

	return S_OK;
}

HRESULT FindUnconnectedPin(CComPtr<IBaseFilter> filter, PIN_DIRECTION pinDir, CComPtr<IPin>& outPin)
{
	HRESULT hr = S_OK;

	CComPtr<IEnumPins> enumPins;
	VALIDATE_HR(filter->EnumPins(&enumPins));
	
	bool isFound = false;
	while (!isFound)
	{
		CComPtr<IPin> pin;
		ULONG numFetched = 0;
		VALIDATE_HR(enumPins->Next(1, &pin, &numFetched));

		if (numFetched == 0) {
			hr = VFW_E_NOT_FOUND;
			break;
		}
		
		VALIDATE_HR(MatchPin(pin, pinDir, false, isFound));
		if (isFound)
		{
			outPin = pin;
		}
	}

	return hr;
}

HRESULT connectFilters(CComPtr<IGraphBuilder> graph, CComPtr<IBaseFilter> sourceBaseFilter, CComPtr<IBaseFilter> targetBaseFilter) {

	CComPtr<IPin> pinOutput;
	VALIDATE_HR(FindUnconnectedPin(sourceBaseFilter, PINDIR_OUTPUT, pinOutput));

	CComPtr<IPin> pinInput;
	VALIDATE_HR(FindUnconnectedPin(targetBaseFilter, PINDIR_INPUT, pinInput));
	
	VALIDATE_HR(graph->Connect(pinOutput, pinInput));

	return S_OK;
}

class CaptureFilter : public CBaseRenderer
{
public:
	CaptureFilter() : CBaseRenderer(CLSID_CaptureFilter, L"CaptureFilter", NULL, &_hr) {
		printf("CaptureFilter()\n");
	}

	virtual ~CaptureFilter() {
		printf("~CaptureFilter()\n");
	}

	virtual HRESULT CheckMediaType(const CMediaType* mtIn)
	{
		printf("CaptureFilter::CheckMediaType()\n");

		// S_OK / S_FALSE

		const GUID* majorType = mtIn->Type();
		const GUID* subType = mtIn->Subtype();

		assert(*majorType == KSDATAFORMAT_TYPE_STREAM);
		assert(*subType == KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT);

		return S_OK;
	}

	virtual HRESULT DoRenderSample(IMediaSample* pMediaSample) {
		printf("CaptureFilter::DoRenderSample()\n");

		long size = pMediaSample->GetSize();		
		long actualSize = pMediaSample->GetActualDataLength();
		printf(" -> size [%ld] / actualSize [%ld] bytes\n", size, actualSize);

		BYTE* buffer;
		VALIDATE_HR(pMediaSample->GetPointer(&buffer));

		printf(" -> [%02x %02x %02x %02x %02x %02x %02x %02x",
			buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
		

		return S_OK;
	}


private:
	HRESULT _hr;
};

void run() {
	// Enumerate all filters with category KSCATEGORY_BDA_NETWORK_TUNER.
	// These filters are usually installed by vendors of hardware tuners
	// when they provide BDA-compatible drivers.
	std::vector<CComPtr<::IMoniker>> tunerMonikers;
	VALIDATE_HR(EnumerateDevicesByClass(KSCATEGORY_BDA_NETWORK_TUNER, tunerMonikers));
	assert(tunerMonikers.size() == 1);

	CComPtr<IMoniker> moniker;
	moniker = tunerMonikers[0];

	// Create instance of Filter Graph Manager
	CComPtr<IGraphBuilder> graph;
	VALIDATE_HR(CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&graph));

	// get control interfaces for the Filter Graph Manager
	CComPtr<IMediaControl> mediaControl;
	CComPtr<IMediaEvent> mediaEvent;
	VALIDATE_HR(graph->QueryInterface(IID_IMediaControl, (void **)&mediaControl));
	VALIDATE_HR(graph->QueryInterface(IID_IMediaEvent, (void **)&mediaEvent));

	// create network provider
	CComPtr<IBDA_NetworkProvider> networkProvider;
	VALIDATE_HR(CoCreateInstance(CLSID_NetworkProvider, NULL,
		CLSCTX_INPROC_SERVER, __uuidof(IBDA_NetworkProvider), (void**)&networkProvider));
	
	// get IBaseFilter interface for network provider
	CComPtr<IBaseFilter> networkProviderBaseFilter;
	VALIDATE_HR(networkProvider->QueryInterface(IID_IBaseFilter, (void **)&networkProviderBaseFilter));

	// get ITuner interface for network provider
	CComPtr<ITuner> networkProviderTuner;
	VALIDATE_HR(networkProvider->QueryInterface(__uuidof(ITuner), (void **)&networkProviderTuner));

	// create instance of tuner from moniker
	CComPtr<IBaseFilter> tunerBaseFilter;
	VALIDATE_HR(moniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&tunerBaseFilter));

	// add network provider to the graph
	VALIDATE_HR(graph->AddFilter(networkProviderBaseFilter, L"NetworkProvider"));
	VALIDATE_HR(graph->AddFilter(tunerBaseFilter, L"Tuner"));
	
	// connect Network Provider to Tuner
	VALIDATE_HR(connectFilters(graph, networkProviderBaseFilter, tunerBaseFilter));
	
	// enumerate Tuning Spaces
	CComPtr<ITuningSpaceContainer> tuningSpaceContainer;
	VALIDATE_HR(CoCreateInstance(__uuidof(SystemTuningSpaces), NULL,
		CLSCTX_INPROC_SERVER, __uuidof(ITuningSpaceContainer), (void**)&tuningSpaceContainer));

	CComPtr<IEnumTuningSpaces> enumTuningSpaces;
	VALIDATE_HR(tuningSpaceContainer->get_EnumTuningSpaces(&enumTuningSpaces));
	
	/// @note this assumes only one tuning space
	CComPtr<ITuningSpace> tuningSpace;
	while (!tuningSpace.p) {
		ULONG numFetched = 0;
		VALIDATE_HR(enumTuningSpaces->Next(1, &tuningSpace, &numFetched));
				
		assert(numFetched > 0);	

		BSTR friendlyName;
		VALIDATE_HR(tuningSpace->get_FriendlyName(&friendlyName));
		printf("FOUND TUNING SPACE: %S\n", friendlyName);
		
		if (FAILED(INVOKE_HR(networkProviderTuner->put_TuningSpace(tuningSpace)))) {			
			tuningSpace.Release();
		}
		else {
			printf("CHOSEN TUNING SPACE: %S\n", friendlyName);
		}

		SysFreeString(friendlyName);
	}

	// Enumerate Receivers
	std::vector<CComPtr<IMoniker>> receiverMonikers;
	VALIDATE_HR(EnumerateDevicesByClass(KSCATEGORY_BDA_RECEIVER_COMPONENT, receiverMonikers));
	assert(1 == receiverMonikers.size());
	CComPtr<IMoniker> receiverMoniker = receiverMonikers[0];

	// Create Receiver from moniker	
	CComPtr<IBaseFilter> baseFilterReceiver;	
	VALIDATE_HR(receiverMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**) &baseFilterReceiver));
	
	// Add Receiver to Graph
	VALIDATE_HR(graph->AddFilter(baseFilterReceiver, L"Receiver"));
	VALIDATE_HR(connectFilters(graph, tunerBaseFilter, baseFilterReceiver));

	// Add Infinite Tee filter (to split output)
	CComPtr<IBaseFilter> baseFilterTee;
	VALIDATE_HR(CoCreateInstance(CLSID_InfTee, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&baseFilterTee));
	VALIDATE_HR(graph->AddFilter(baseFilterTee, L"Tee"));
	VALIDATE_HR(connectFilters(graph, baseFilterReceiver, baseFilterTee));

	// Add CaptureFilter (where we can receive the transport stream)
	CComPtr<IBaseFilter> baseFilterCaptureFilter;
	baseFilterCaptureFilter = new CaptureFilter();

	VALIDATE_HR(graph->AddFilter(baseFilterCaptureFilter, L"CaptureFilter"));
	VALIDATE_HR(connectFilters(graph, baseFilterTee, baseFilterCaptureFilter));


	// add MPEG-2 De-Multiplexer (so that graph can run)
	CComPtr<IBaseFilter> baseFilterDemux;
	VALIDATE_HR(CoCreateInstance(CLSID_MPEG2Demultiplexer, NULL,
		CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&baseFilterDemux));

	VALIDATE_HR(graph->AddFilter(baseFilterDemux, L"MPEG2-Demux (vestigial)"));
	VALIDATE_HR(connectFilters(graph, baseFilterTee, baseFilterDemux));

	// enumerate Transport Information Filters (TIF)
	std::vector<CComPtr<IMoniker>> transportInformationMonikers;
	VALIDATE_HR(EnumerateDevicesByClass(KSCATEGORY_BDA_TRANSPORT_INFORMATION, transportInformationMonikers));

	for (auto transportInformationMoniker : transportInformationMonikers) {
		CComPtr<IBaseFilter> baseFilterTransportInformation;
		VALIDATE_HR(transportInformationMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**) &baseFilterTransportInformation));

		VALIDATE_HR(graph->AddFilter(baseFilterTransportInformation, L"TIF"));

		if (SUCCEEDED(connectFilters(graph, baseFilterDemux, baseFilterTransportInformation))) {
			printf("TRANSPORT INFORMATION FILTER CONNECTED\n");
			break;
		}
		else 
		{
			VALIDATE_HR(graph->RemoveFilter(baseFilterTransportInformation));
		}
	}

	/// @todo Tune to the BBC transmission, and get some packets!
	CComPtr<ITuneRequest> tuneRequest;
	VALIDATE_HR(tuningSpace->CreateTuneRequest(&tuneRequest));

	CComPtr<IDVBTuneRequest> dvbTuneRequest;
	VALIDATE_HR(tuneRequest->QueryInterface(__uuidof(IDVBTuneRequest), (void**)&dvbTuneRequest));

	VALIDATE_HR(dvbTuneRequest->put_ONID(-1));
	VALIDATE_HR(dvbTuneRequest->put_TSID(-1));
	VALIDATE_HR(dvbTuneRequest->put_SID(-1));

	
	CComPtr<IDVBTLocator> dvbtLocator;
	VALIDATE_HR(CoCreateInstance(__uuidof(DVBTLocator), NULL,
		CLSCTX_INPROC_SERVER, __uuidof(IDVBTLocator), (void**)&dvbtLocator));
	
	VALIDATE_HR(dvbtLocator->put_CarrierFrequency(490 * 1000));		// kHz
	VALIDATE_HR(dvbtLocator->put_Modulation(BDA_MOD_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_Bandwidth(8));
	VALIDATE_HR(dvbtLocator->put_Guard(BDA_GUARD_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_LPInnerFEC(BDA_FEC_VITERBI));
	VALIDATE_HR(dvbtLocator->put_LPInnerFECRate(BDA_BCC_RATE_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_Mode(BDA_XMIT_MODE_NOT_DEFINED));
	VALIDATE_HR(dvbtLocator->put_HAlpha(BDA_HALPHA_NOT_DEFINED));

		
	CComPtr<IDigitalLocator> digitalLocator = dvbtLocator;

	VALIDATE_HR(tuneRequest->put_Locator(digitalLocator));
	VALIDATE_HR(networkProviderTuner->put_TuneRequest(tuneRequest));


	printf("start the graph\n");
	VALIDATE_HR(mediaControl->Run());

	printf("wait 10 seconds\n");
	Sleep(10 * 1000);
	

	printf("\nSUCCESSFULLY finished execution\n\n");

}

