/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "ChipObject.h"
#include "HAL/cpp/priority_mutex.h"

namespace hal {
constexpr long kTimebase = 40000000;  ///< 40 MHz clock
constexpr long kDefaultOversampleBits = 0;
constexpr long kDefaultAverageBits = 7;
constexpr float kDefaultSampleRate = 50000.0;
constexpr uint32_t kAnalogInputPins = 8;
constexpr uint32_t kAnalogOutputPins = 2;
constexpr uint32_t kAccumulatorNumChannels = 2;
static const uint32_t kAccumulatorChannels[] = {0, 1};

extern tAI* analogInputSystem;
extern tAO* analogOutputSystem;
extern priority_recursive_mutex analogRegisterWindowMutex;

struct AnalogPort {
  uint8_t pin;
  tAccumulator* accumulator;
};

uint32_t getAnalogNumActiveChannels(int32_t* status);
uint32_t getAnalogNumChannelsToActivate(int32_t* status);
void setAnalogNumChannelsToActivate(uint32_t channels);
void initializeAnalog(int32_t* status);

extern bool analogSystemInitialized;
}
