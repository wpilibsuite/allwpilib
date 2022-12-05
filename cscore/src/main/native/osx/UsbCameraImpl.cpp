// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Instance.h"
#include "cscore_cpp.h"

namespace cs {

CS_Source CreateUsbCameraDev(std::string_view name, int dev,
                             CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  WPI_ERROR(Instance::GetInstance().logger,
            "USB Camera support not implemented for macOS");
  return 0;
}

CS_Source CreateUsbCameraPath(std::string_view name, std::string_view path,
                              CS_Status* status) {
  *status = CS_INVALID_HANDLE;
  WPI_ERROR(Instance::GetInstance().logger,
            "USB Camera support not implemented for macOS");
  return 0;
}

void SetUsbCameraPath(CS_Source source, std::string_view path,
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
  WPI_ERROR(Instance::GetInstance().logger,
            "USB Camera support not implemented for macOS");
  return std::vector<UsbCameraInfo>{};
}

}  // namespace cs
