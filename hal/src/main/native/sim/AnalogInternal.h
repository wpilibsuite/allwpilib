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
constexpr int32_t TIME_BASE = 40000000;  ///< 40 MHz clock
constexpr int32_t DEFAULT_OVERSAMPLE_BITS = 0;
constexpr int32_t DEFAULT_AVERAGE_BITS = 7;
constexpr double DEFAULT_SAMPLE_RATE = 50000.0;
static constexpr uint32_t ACCUMULATOR_CHANNELS[] = {0, 1};

struct AnalogPort {
  uint8_t channel;
  bool isAccumulator;
  std::string previousAllocation;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             NUM_ANALOG_INPUTS, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;
}  // namespace hal
