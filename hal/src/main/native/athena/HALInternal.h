// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {
void ReleaseFPGAInterrupt(int32_t interruptNumber);
const char* HAL_GetLastError(int32_t* status);

}  // namespace hal
