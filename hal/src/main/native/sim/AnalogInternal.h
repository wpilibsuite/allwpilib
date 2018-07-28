/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include "PortsInternal.h"
#include "hal/Ports.h"
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
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;

int32_t GetAnalogTriggerInputIndex(HAL_AnalogTriggerHandle handle,
                                   int32_t* status);
}  // namespace hal
