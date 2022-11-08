// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace hal {

constexpr int32_t kSystemClockTicksPerMicrosecond = 40;
constexpr int32_t kDutyCycleScaleFactor = 4e7 - 1;

}  // namespace hal
