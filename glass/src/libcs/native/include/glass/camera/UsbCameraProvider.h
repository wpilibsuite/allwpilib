// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cscore_cpp.h>
#include <wpi/SafeThread.h>

#include "glass/camera/CameraProviderBase.h"

namespace glass {

class Storage;

/**
 * Provider for CameraServer models and views for USB cameras.
 */
class UsbCameraProvider : public CameraProviderBase {
 public:
  explicit UsbCameraProvider(Storage& storage);
  ~UsbCameraProvider() override;

  /**
   * Displays menu contents.
   */
  void DisplayMenu() override;

 private:
  void Update() override;
  void InitCamera(SourceInfo* info) override;

  struct UsbInfoThread;
  wpi::SafeThreadOwner<UsbInfoThread> m_usbInfoThread;

  CS_ListenerPoller m_poller;
};

}  // namespace glass
