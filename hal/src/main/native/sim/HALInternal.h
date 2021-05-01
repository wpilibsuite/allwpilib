// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <wpi/Twine.h>

namespace hal {
void SetLastError(int32_t* status, const wpi::Twine& value);
void SetLastErrorIndexOutOfRange(int32_t* status, const wpi::Twine& message,
                                 int32_t minimum, int32_t maximum,
                                 int32_t channel);
void SetLastErrorPreviouslyAllocated(int32_t* status, const wpi::Twine& message,
                                     int32_t channel,
                                     const wpi::Twine& previousAllocation);
}  // namespace hal
