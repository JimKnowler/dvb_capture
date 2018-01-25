#include <Dshow.h>
#include <assert.h>
#include <atlbase.h>
#include <vector>

#define LOG_HR( _str, _hr ) printf("0x%08X : %s\n", _hr, _str)
#define INVOKE_HR( _com ) InvokeHR( _com, #_com )
#define VALIDATE_HR( _com ) { HRESULT _hr = (_com); LOG_HR( #_com, _hr); assert(SUCCEEDED(_hr)); }

HRESULT InvokeHR(HRESULT hr, const char* str);
HRESULT EnumerateDevicesByClass(const ::CLSID& clsid, std::vector <CComPtr <::IMoniker>>& monikers);
HRESULT IsPinConnected(CComPtr<IPin> pin, bool& outResult);
HRESULT IsPinDirection(CComPtr<IPin> pin, PIN_DIRECTION dir, bool& outResult);
HRESULT MatchPin(CComPtr<IPin> pin, PIN_DIRECTION direction, bool shouldBeConnected, bool& outResult);
HRESULT FindUnconnectedPin(CComPtr<IBaseFilter> filter, PIN_DIRECTION pinDir, CComPtr<IPin>& outPin);
HRESULT connectFilters(CComPtr<IGraphBuilder> graph, CComPtr<IBaseFilter> sourceBaseFilter, CComPtr<IBaseFilter> targetBaseFilter);
