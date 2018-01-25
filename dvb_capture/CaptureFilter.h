typedef wchar_t *PTCHAR;
#include <streams.h>

#include <functional>
typedef std::function<void(const BYTE*, long)> CallbackTransportStream;

class CaptureFilter : public CBaseRenderer
{
public:
	CaptureFilter();

	bool setCallbackTransportStream(CallbackTransportStream callback);

public: // CBaseRenderer
	~CaptureFilter() override;
	HRESULT CheckMediaType(const CMediaType* mtIn) override;
	HRESULT DoRenderSample(IMediaSample* pMediaSample) override;

private:
	HRESULT _hr;

	CallbackTransportStream callback;
};