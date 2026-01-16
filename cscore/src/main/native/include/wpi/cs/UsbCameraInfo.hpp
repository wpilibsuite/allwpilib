// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <vector>

namespace wpi::cs {

/**
 * USB camera information
 */
struct UsbCameraInfo {
  /** Device number (e.g. N in '/dev/videoN' on Linux) */
  int dev = -1;
  /** Path to device if available (e.g. '/dev/video0' on Linux) */
  std::string path;
  /** Vendor/model name of the camera as provided by the USB driver */
  std::string name;
  /** Other path aliases to device (e.g. '/dev/v4l/by-id/...' etc on Linux) */
  std::vector<std::string> otherPaths;
  /** USB Vendor Id */
  int vendorId = -1;
  /** USB Product Id */
  int productId = -1;
};

}  // namespace wpi::cs
