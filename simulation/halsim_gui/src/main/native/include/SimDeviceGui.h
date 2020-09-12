/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/SimDevice.h>

namespace glass {
class DataSource;
class DeviceTreeModel;
}  // namespace glass

namespace halsimgui {

class SimDeviceGui {
 public:
  static void Initialize();
  static glass::DataSource* GetValueSource(HAL_SimValueHandle handle);
  static glass::DeviceTreeModel& GetDeviceTree();
};

}  // namespace halsimgui
