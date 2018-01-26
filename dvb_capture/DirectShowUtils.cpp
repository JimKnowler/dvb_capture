#include "stdafx.h"

#include "DirectShowUtils.h"

HRESULT invokeHR(HRESULT hr, const char* str) {
	LOG_HR(str, hr);
	return hr;
}

HRESULT enumerateDevicesByClass(const ::CLSID& clsid, std::vector <CComPtr <::IMoniker>>& monikers)
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

HRESULT isPinConnected(CComPtr<IPin> pin, bool& outResult)
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

HRESULT isPinDirection(CComPtr<IPin> pin, PIN_DIRECTION dir, bool& outResult)
{
	PIN_DIRECTION pinDir;
	VALIDATE_HR(pin->QueryDirection(&pinDir));

	outResult = (pinDir == dir);

	return S_OK;
}

HRESULT matchPin(CComPtr<IPin> pin, PIN_DIRECTION direction, bool shouldBeConnected, bool& outResult)
{
	bool isMatch = false;
	bool isConnected = false;

	VALIDATE_HR(isPinConnected(pin, isConnected));

	if (isConnected == shouldBeConnected)
	{
		VALIDATE_HR(isPinDirection(pin, direction, isMatch));
	}

	outResult = isMatch;

	return S_OK;
}

HRESULT findUnconnectedPin(CComPtr<IBaseFilter> filter, PIN_DIRECTION pinDir, CComPtr<IPin>& outPin)
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

		VALIDATE_HR(matchPin(pin, pinDir, false, isFound));
		if (isFound)
		{
			outPin = pin;
		}
	}

	return hr;
}

HRESULT connectFilters(CComPtr<IGraphBuilder> graph, CComPtr<IBaseFilter> sourceBaseFilter, CComPtr<IBaseFilter> targetBaseFilter) {

	CComPtr<IPin> pinOutput;
	VALIDATE_HR(findUnconnectedPin(sourceBaseFilter, PINDIR_OUTPUT, pinOutput));

	CComPtr<IPin> pinInput;
	VALIDATE_HR(findUnconnectedPin(targetBaseFilter, PINDIR_INPUT, pinInput));

	VALIDATE_HR(graph->Connect(pinOutput, pinInput));

	return S_OK;
}