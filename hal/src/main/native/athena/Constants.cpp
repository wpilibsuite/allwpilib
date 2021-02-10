// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/Constants.h"

#include "ConstantsInternal.h"

using namespace hal;

namespace hal::init {
void InitializeConstants() {}
}  // namespace hal::init

extern "C" {

int32_t HAL_GetSystemClockTicksPerMicrosecond(void) {
  return kSystemClockTicksPerMicrosecond;
}

}  // extern "C"
