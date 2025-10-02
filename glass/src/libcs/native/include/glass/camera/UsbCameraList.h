// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include <cscore_cpp.h>
#include <wpi/SafeThread.h>

namespace glass {

class Storage;

/**
 * Provider for CameraServer models and views for USB cameras.
 */
class UsbCameraList {
 public:
  explicit UsbCameraList();
  ~UsbCameraList();

  UsbCameraList(const UsbCameraList&) = delete;
  UsbCameraList& operator=(const UsbCameraList&) = delete;

  void Update();

  /**
   * Displays menu contents.
   *
   * @return camera path to add, if not empty
   */
  std::string DisplayMenu();

 private:
  struct UsbInfoThread;
  wpi::SafeThreadOwner<UsbInfoThread> m_usbInfoThread;

  CS_ListenerPoller m_poller;
};

}  // namespace glass
