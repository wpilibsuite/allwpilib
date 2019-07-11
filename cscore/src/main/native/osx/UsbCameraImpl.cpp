/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
