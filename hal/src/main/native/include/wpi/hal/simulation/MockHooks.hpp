// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/hal/simulation/MockHooks.h"
#include "wpi/hal/DriverStationTypes.hpp"

namespace wpi::hal::sim {
inline void SetProgramState(const ControlWord& controlWord) {
  HALSIM_SetProgramState(controlWord.GetValue());
}

inline ControlWord GetProgramState() {
  HAL_ControlWord word;
  HALSIM_GetProgramState(&word);
  return ControlWord{word};
}
}  // namespace wpi::hal::sim
