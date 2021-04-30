// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/Twine.h>

namespace hal {
void SetLastError(int32_t* status, const wpi::Twine& value);
}  // namespace hal
