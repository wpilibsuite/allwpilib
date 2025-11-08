// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/SimDevice.h"

namespace wpi::glass {
class DataSource;
class DeviceTreeModel;
}  // namespace glass

namespace halsimgui {

class SimDeviceGui {
 public:
  static void Initialize();
  static wpi::glass::DataSource* GetValueSource(HAL_SimValueHandle handle);
  static wpi::glass::DeviceTreeModel& GetDeviceTree();
};

}  // namespace halsimgui
