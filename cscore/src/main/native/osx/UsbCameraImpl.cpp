// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cscore_cpp.h"

namespace cs {

CS_Source CreateUsbCameraDev(const wpi::Twine& name, int dev,
                             CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  return 0;
}

CS_Source CreateUsbCameraPath(const wpi::Twine& name, const wpi::Twine& path,
                              CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  return 0;
}

void SetUsbCameraPath(CS_Source source, const wpi::Twine& path,
                      CS_Status* status) {
  *status = CS_INVALID_HANDLE;
}

std::string GetUsbCameraPath(CS_Source source, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  return std::string{};
}

UsbCameraInfo GetUsbCameraInfo(CS_Source source, CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  return UsbCameraInfo{};
}

std::vector<UsbCameraInfo> EnumerateUsbCameras(CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  return std::vector<UsbCameraInfo>{};
}

}  // namespace cs
