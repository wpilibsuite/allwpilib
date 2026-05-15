// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string_view>

#include "wpi/hal/Types.h"

namespace wpi::hal {
[[nodiscard]]
HAL_Status MakeError(HAL_Status status, std::string_view value);

[[nodiscard]]
HAL_Status MakeErrorIndexOutOfRange(HAL_Status status, std::string_view message,
                                    int32_t minimum, int32_t maximum,
                                    int32_t channel);

[[nodiscard]]
HAL_Status MakeErrorPreviouslyAllocated(HAL_Status status,
                                        std::string_view message,
                                        int32_t channel,
                                        std::string_view previousAllocation);
}  // namespace wpi::hal
