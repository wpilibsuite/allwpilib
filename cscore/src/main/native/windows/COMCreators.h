#pragma once

#include <mfreadwrite.h>
#include <mfidl.h>

namespace cs {

class UsbCameraImpl;

class SourceReaderCB : public IMFSourceReaderCallback
{
public:
    SourceReaderCB(HWND hwnd, cs::UsbCameraImpl* source) :
      m_nRefCount(1), m_hwnd(hwnd), m_source(source)
    {
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
    HWND                m_hwnd;
    UsbCameraImpl*      m_source;
};

SourceReaderCB* CreateSourceReaderCB(HWND hwnd, cs::UsbCameraImpl* source);
IMFSourceReader* CreateSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback);
IMFMediaSource* CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink);
HRESULT ConfigureDecoder(IMFSourceReader* sourceReader, DWORD dwStreamIndex);
}
