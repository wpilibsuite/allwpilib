#pragma once

#include <mfreadwrite.h>
#include <mfidl.h>

namespace cs {

class UsbCameraImpl;

ComPtr<IMFSourceReaderCallback> CreateSourceReaderCB(HWND hwnd, cs::UsbCameraImpl* source);
ComPtr<IMFSourceReader> CreateSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback);
ComPtr<IMFMediaSource> CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink);
}
