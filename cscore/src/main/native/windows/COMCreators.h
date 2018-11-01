/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>

#include <memory>

#include "ComPtr.h"
#include "cscore_cpp.h"

namespace cs {

class UsbCameraImpl;

// Source callback used by the source reader.
// COM object, so it needs a to ref count itself.
class SourceReaderCB : public IMFSourceReaderCallback {
 public:
  explicit SourceReaderCB(std::weak_ptr<cs::UsbCameraImpl> source,
                          const cs::VideoMode& mode);
  void SetVideoMode(const VideoMode& mode) { m_mode = mode; }

  STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();

  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*);
  STDMETHODIMP OnFlush(DWORD);
  STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                            DWORD dwStreamFlags, LONGLONG llTimestamp,
                            IMFSample* pSample  // Can be NULL
  );

  void InvalidateCapture() { m_source = std::weak_ptr<cs::UsbCameraImpl>(); }

 private:
  // Destructor is private. Caller should call Release.
  virtual ~SourceReaderCB() {}
  void NotifyError(HRESULT hr);

  ULONG m_nRefCount;
  std::weak_ptr<cs::UsbCameraImpl> m_source;
  cs::VideoMode m_mode;
};

ComPtr<SourceReaderCB> CreateSourceReaderCB(
    std::weak_ptr<cs::UsbCameraImpl> source, const cs::VideoMode& mode);
ComPtr<IMFSourceReader> CreateSourceReader(IMFMediaSource* mediaSource,
                                           IMFSourceReaderCallback* callback);
ComPtr<IMFMediaSource> CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink);
}  // namespace cs
