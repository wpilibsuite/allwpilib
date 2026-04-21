// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include "PortsInternal.hpp"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/hal/handles/IndexedHandleResource.hpp"

namespace wpi::hal {

struct AnalogPort {
  uint8_t channel;
  std::string previousAllocation;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, wpi::hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::ANALOG_INPUT>*
    analogInputHandles;
}  // namespace wpi::hal
