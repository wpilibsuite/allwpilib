// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Constants.h"

#include "ConstantsInternal.h"

using namespace wpi::hal;

namespace wpi::hal::init {
void InitializeConstants() {}
}  // namespace wpi::hal::init

extern "C" {
int32_t HAL_GetSystemClockTicksPerMicrosecond(void) {
  return kSystemClockTicksPerMicrosecond;
}
}  // extern "C"
