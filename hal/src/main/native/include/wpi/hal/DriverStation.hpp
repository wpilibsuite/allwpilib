// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.hpp"

namespace wpi::hal {
inline ControlWord GetControlWord() {
  HAL_ControlWord word;
  HAL_GetControlWord(&word);
  return ControlWord{word};
}

inline ControlWord GetUncachedControlWord() {
  HAL_ControlWord word;
  HAL_GetUncachedControlWord(&word);
  return ControlWord{word};
}
}  // namespace wpi::hal
