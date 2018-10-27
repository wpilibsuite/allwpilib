/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <mfidl.h>
#include <mfreadwrite.h>

namespace cs {

class UsbCameraImpl;

ComPtr<IMFSourceReaderCallback> CreateSourceReaderCB(HWND hwnd,
                                                     cs::UsbCameraImpl* source);
ComPtr<IMFSourceReader> CreateSourceReader(IMFMediaSource* mediaSource,
                                           IMFSourceReaderCallback* callback);
ComPtr<IMFMediaSource> CreateVideoCaptureDevice(LPCWSTR pszSymbolicLink);
}  // namespace cs
