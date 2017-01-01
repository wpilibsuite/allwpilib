/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include "HAL/ChipObject.h"
#include "HAL/Ports.h"
#include "HAL/cpp/priority_mutex.h"
#include "HAL/handles/IndexedHandleResource.h"
#include "PortsInternal.h"

namespace hal {
constexpr int32_t kTimebase = 40000000;  ///< 40 MHz clock
constexpr int32_t kDefaultOversampleBits = 0;
constexpr int32_t kDefaultAverageBits = 7;
constexpr double kDefaultSampleRate = 50000.0;
static const uint32_t kAccumulatorChannels[] = {0, 1};

extern std::unique_ptr<tAI> analogInputSystem;
extern std::unique_ptr<tAO> analogOutputSystem;
extern priority_recursive_mutex analogRegisterWindowMutex;
extern bool analogSampleRateSet;

struct AnalogPort {
  uint8_t channel;
  std::unique_ptr<tAccumulator> accumulator;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::AnalogInput>
    analogInputHandles;

int32_t getAnalogNumActiveChannels(int32_t* status);
int32_t getAnalogNumChannelsToActivate(int32_t* status);
void setAnalogNumChannelsToActivate(int32_t channels);
void setAnalogSampleRate(double samplesPerSecond, int32_t* status);
void initializeAnalog(int32_t* status);
}  // namespace hal
