/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cstring>

#include <hal/DriverStationTypes.h>

namespace halsim {

struct DSCommJoystickPacket {
  HAL_JoystickAxes axes;
  HAL_JoystickButtons buttons;
  HAL_JoystickPOVs povs;
  HAL_JoystickDescriptor descriptor;

  void ResetUdp() {
    std::memset(&axes, 0, sizeof(axes));
    std::memset(&buttons, 0, sizeof(buttons));
    std::memset(&povs, 0, sizeof(povs));
  }

  void ResetTcp() { std::memset(&descriptor, 0, sizeof(descriptor)); }
};

}  // namespace halsim
