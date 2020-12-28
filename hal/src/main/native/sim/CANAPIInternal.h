// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "hal/Types.h"

namespace hal::can {
int32_t GetCANModuleFromHandle(HAL_CANHandle handle, int32_t* status);
}  // namespace hal::can
