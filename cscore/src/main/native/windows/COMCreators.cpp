#include <shlwapi.h>
#include <Windows.h>
#include <windowsx.h>
#include <mfidl.h>
#include <mfapi.h>
#include <iostream>

#include <UsbCameraImplWindows.h>

//https://github.com/opencv/opencv/blob/master/modules/videoio/src/cap_msmf.cpp

#include <mfidl.h>
#include <mfapi.h>
#include <Dbt.h>
#include <ks.h>
#include <ksmedia.h>
#include <mfreadwrite.h>

#include "COMCreators.h"
#include "ComPtr.h"

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace cs {

HRESULT SourceReaderCB::OnReadSample(
    HRESULT hrStatus,
    DWORD dwStreamIndex,
    DWORD dwStreamFlags,
    LONGLONG llTimestamp,
    IMFSample *pSample      // Can be NULL
    )
{
    EnterCriticalSection(&m_critsec);

    if (SUCCEEDED(hrStatus))
    {
        if (pSample)
        {
            // Do something with the sample.
            wprintf(L"Frame @ %I64d\n", llTimestamp);

            m_lastSample = pSample;
        }
    }
    else
    {
        // Streaming error.
        NotifyError(hrStatus);
    }

    if (MF_SOURCE_READERF_ENDOFSTREAM & dwStreamFlags)
    {
        // Reached the end of the stream.
        m_bEOS = TRUE;
    }
    m_hrStatus = hrStatus;

    LeaveCriticalSection(&m_critsec);
    PostMessage(m_hwnd, 4488, 0, 0);
    return S_OK;
}

SourceReaderCB* CreateSourceReaderCB(HWND hwnd) {
  return new SourceReaderCB(hwnd);
}

IMFMediaSource* CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink)
{
    IMFAttributes *pAttributes = NULL;
    IMFMediaSource *pSource = NULL;

    HRESULT hr = MFCreateAttributes(&pAttributes, 2);

    // Set the device type to video.
    if (SUCCEEDED(hr))
    {
        hr = pAttributes->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
            );
    }


    // Set the symbolic link.
    if (SUCCEEDED(hr))
    {
        hr = pAttributes->SetString(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
            pszSymbolicLink
            );
    }

    if (SUCCEEDED(hr))
    {
        hr = MFCreateDeviceSource(pAttributes, &pSource);
    }

    SafeRelease(&pAttributes);
    return pSource;
}

IMFSourceReader* CreateSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback) {
  HRESULT hr = S_OK;
    IMFAttributes *pAttributes = NULL;

    IMFSourceReader* sourceReader = NULL;

    hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pAttributes->SetUINT32(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, TRUE);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = MFCreateSourceReaderFromMediaSource(mediaSource, pAttributes, &sourceReader);

done:
    SafeRelease(&pAttributes);
    return sourceReader;
}

 HRESULT ConfigureDecoder(IMFSourceReader *pReader, DWORD dwStreamIndex)
{
    IMFMediaType *pNativeType = NULL;
    IMFMediaType *pType = NULL;

    // Find the native format of the stream.
    HRESULT hr = pReader->GetNativeMediaType(dwStreamIndex, 0, &pNativeType);
    if (FAILED(hr))
    {
        std::cout << "Failed getnative\n";
        return hr;
    }

    GUID majorType, subtype;

    // Find the major type.
    hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &majorType);
    if (FAILED(hr))
    {
        std::cout << "Failed getguid\n";
        goto done;
    }

    // Define the output type.
    hr = MFCreateMediaType(&pType);
    if (FAILED(hr))
    {
        std::cout << "Failed nediatype\n";
        goto done;
    }

    hr = pType->SetGUID(MF_MT_MAJOR_TYPE, majorType);
    if (FAILED(hr))
    {
        std::cout << "Failed setguid major\n";
        goto done;
    }

    // Select a subtype.
    if (majorType == MFMediaType_Video)
    {
        std::cout << "Setting media type\n";
        subtype= MFVideoFormat_RGB24;
    }
    else if (majorType == MFMediaType_Audio)
    {
        subtype = MFAudioFormat_PCM;
    }
    else
    {
        // Unrecognized type. Skip.
        goto done;
    }

    hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);
    if (FAILED(hr))
    {
        std::cout << "Failed setguid\n";
        goto done;
    }

    // Set the uncompressed format.
    hr = pReader->SetCurrentMediaType(dwStreamIndex, NULL, pType);
    if (FAILED(hr))
    {
        std::cout << "Failed setformat\n";
        goto done;
    }

done:
    SafeRelease(&pNativeType);
    SafeRelease(&pType);
    return hr;
}

}
