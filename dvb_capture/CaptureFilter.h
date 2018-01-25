typedef wchar_t *PTCHAR;
#include <streams.h>

class CaptureFilter : public CBaseRenderer
{
public:
	CaptureFilter();

	virtual ~CaptureFilter();

	virtual HRESULT CheckMediaType(const CMediaType* mtIn);

	virtual HRESULT DoRenderSample(IMediaSample* pMediaSample);


private:
	HRESULT _hr;
};