#include "stdafx.h"
#include "CaptureFilter.h"
#include "DirectShowUtils.h"

#include <Tuner.h>
#include <ks.h>
#include <ksmedia.h>
#include <bdamedia.h>

#define APP_GUID(_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const ::GUID _name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

APP_GUID(CLSID_CaptureFilter, 0xfd628dda, 0x002a, 0x11e8, 0xba, 0x89, 0x0e, 0xd5, 0xf8, 0x9f, 0x71, 0x8b);

const auto kNullCallbackTransportStream = [](const BYTE*, long) {};

CaptureFilter::CaptureFilter() : CBaseRenderer(CLSID_CaptureFilter, L"CaptureFilter", NULL, &m_hr) {
	m_callback = kNullCallbackTransportStream;
}

CaptureFilter::~CaptureFilter() {
}

bool CaptureFilter::setCallbackTransportStream(CallbackTransportStream inCallback)
{
	m_callback = inCallback;

	return true;
}

HRESULT CaptureFilter::CheckMediaType(const CMediaType* mtIn)
{
	const GUID* majorType = mtIn->Type();
	const GUID* subType = mtIn->Subtype();

	assert(*majorType == KSDATAFORMAT_TYPE_STREAM);
	assert(*subType == KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT);

	return S_OK;
}

HRESULT CaptureFilter::DoRenderSample(IMediaSample* pMediaSample) {

	long actualSize = pMediaSample->GetActualDataLength();

	BYTE* buffer;
	VALIDATE_HR(pMediaSample->GetPointer(&buffer));

	m_callback(buffer, actualSize);	

	return S_OK;
}
