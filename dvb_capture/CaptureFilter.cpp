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

CaptureFilter::CaptureFilter() : CBaseRenderer(CLSID_CaptureFilter, L"CaptureFilter", NULL, &_hr) {
	printf("CaptureFilter()\n");
}

CaptureFilter::~CaptureFilter() {
	printf("~CaptureFilter()\n");
}

HRESULT CaptureFilter::CheckMediaType(const CMediaType* mtIn)
{
	printf("CaptureFilter::CheckMediaType()\n");

	// S_OK / S_FALSE

	const GUID* majorType = mtIn->Type();
	const GUID* subType = mtIn->Subtype();

	assert(*majorType == KSDATAFORMAT_TYPE_STREAM);
	assert(*subType == KSDATAFORMAT_SUBTYPE_BDA_MPEG2_TRANSPORT);

	return S_OK;
}

HRESULT CaptureFilter::DoRenderSample(IMediaSample* pMediaSample) {
	printf("CaptureFilter::DoRenderSample()\n");

	long size = pMediaSample->GetSize();
	long actualSize = pMediaSample->GetActualDataLength();
	printf(" -> size [%ld] / actualSize [%ld] bytes\n", size, actualSize);

	BYTE* buffer;
	VALIDATE_HR(pMediaSample->GetPointer(&buffer));

	printf(" -> [%02x %02x %02x %02x %02x %02x %02x %02x\n",
		buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);


	return S_OK;
}
