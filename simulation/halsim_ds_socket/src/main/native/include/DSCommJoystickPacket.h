// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
