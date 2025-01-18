// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/LEDs.h"

#include <unistd.h>

#include <cstring>
#include <fstream>

#include <fmt/format.h>
#include <fmt/std.h>
#include <wpi/fs.h>

#include "HALInternal.h"
#include "hal/Errors.h"

namespace hal::init {

void InitializeLEDs() {}
}  // namespace hal::init

extern "C" {
void HAL_SetRadioLEDState(HAL_RadioLEDState state, int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return;
}

HAL_RadioLEDState HAL_GetRadioLEDState(int32_t* status) {
  *status = HAL_HANDLE_ERROR;
  return HAL_RadioLED_kOff;
}
}  // extern "C"
