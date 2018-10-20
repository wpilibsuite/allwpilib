#pragma once

#include <mfreadwrite.h>
#include "UsbCameraImplWindows.h"
#include <mfidl.h>

namespace cs {
class SourceReaderCB : public IMFSourceReaderCallback
{
public:
    SourceReaderCB(HWND hwnd) :
      m_nRefCount(1), m_bEOS(FALSE), m_hrStatus(S_OK), m_hwnd(hwnd)
    {
        InitializeCriticalSection(&m_critsec);
    }

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(SourceReaderCB, IMFSourceReaderCallback),
            { 0 },
        };
        return QISearch(this, qit, iid, ppv);
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_nRefCount);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG uCount = InterlockedDecrement(&m_nRefCount);
        if (uCount == 0)
        {
            delete this;
        }
        return uCount;
    }

    // IMFSourceReaderCallback methods
    STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
        DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample *pSample);

    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *)
    {
        return S_OK;
    }

    STDMETHODIMP OnFlush(DWORD)
    {
        return S_OK;
    }

public:
    HRESULT Wait(DWORD dwMilliseconds, BOOL *pbEOS)
    {
        *pbEOS = FALSE;

        DWORD dwResult = WaitForSingleObject(m_hEvent, dwMilliseconds);
        if (dwResult == WAIT_TIMEOUT)
        {
            return E_PENDING;
        }
        else if (dwResult != WAIT_OBJECT_0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        *pbEOS = m_bEOS;
        return m_hrStatus;
    }

    _ComPtr<IMFSample> GetLatestSample() {
        _ComPtr<IMFSample> videoSample = m_lastSample;
        m_lastSample.Release();
        return videoSample;
    }

private:

    // Destructor is private. Caller should call Release.
    virtual ~SourceReaderCB()
    {
    }

    void NotifyError(HRESULT hr)
    {
        wprintf(L"Source Reader error: 0x%X\n", hr);
    }

private:
    long                m_nRefCount;        // Reference count.
    CRITICAL_SECTION    m_critsec;
    HANDLE              m_hEvent;
    BOOL                m_bEOS;
    HRESULT             m_hrStatus;
    HWND                m_hwnd;

    _ComPtr<IMFSample>  m_lastSample;
};

SourceReaderCB* CreateSourceReaderCB(HWND hwnd);
IMFSourceReader* CreateSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback);
IMFMediaSource* CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink);
HRESULT ConfigureDecoder(IMFSourceReader* sourceReader, DWORD dwStreamIndex);
}
