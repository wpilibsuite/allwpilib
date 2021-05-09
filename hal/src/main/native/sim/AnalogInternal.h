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
constexpr int32_t kTimebase = 40000000;  ///< 40 MHz clock
constexpr int32_t kDefaultOversampleBits = 0;
constexpr int32_t kDefaultAverageBits = 7;
constexpr double kDefaultSampleRate = 50000.0;
static constexpr uint32_t kAccumulatorChannels[] = {0, 1};

struct AnalogPort {
  uint8_t channel;
  bool isAccumulator;
  std::string previousAllocation;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;

int32_t GetAnalogTriggerInputIndex(HAL_AnalogTriggerHandle handle,
                                   int32_t* status);
}  // namespace hal
