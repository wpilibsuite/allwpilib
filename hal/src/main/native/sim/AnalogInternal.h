// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include "PortsInternal.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/IndexedHandleResource.h"

namespace hal {

struct AnalogPort {
  uint8_t channel;
  std::string previousAllocation;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;
}  // namespace hal
