/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include <wpi/mutex.h>

#include "PortsInternal.h"
#include "hal/ChipObject.h"
#include "hal/Ports.h"
#include "hal/handles/IndexedHandleResource.h"

namespace hal {

constexpr int32_t kTimebase = 40000000;  ///< 40 MHz clock
constexpr int32_t kDefaultOversampleBits = 0;
constexpr int32_t kDefaultAverageBits = 7;
constexpr double kDefaultSampleRate = 50000.0;
static constexpr uint32_t kAccumulatorChannels[] = {0, 1};

extern std::unique_ptr<tAI> analogInputSystem;
extern std::unique_ptr<tAO> analogOutputSystem;
extern wpi::mutex analogRegisterWindowMutex;
extern bool analogSampleRateSet;

struct AnalogPort {
  uint8_t channel;
  std::unique_ptr<tAccumulator> accumulator;
};

extern IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                             kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;

/**
 * Initialize the analog System.
 */
void initializeAnalog(int32_t* status);

/**
 * Return the number of channels on the module in use.
 *
 * @return Active channels.
 */
int32_t getAnalogNumActiveChannels(int32_t* status);

/**
 * Set the number of active channels.
 *
 * Store the number of active channels to set.  Don't actually commit to
 * hardware
 * until SetSampleRate().
 *
 * @param channels Number of active channels.
 */
void setAnalogNumChannelsToActivate(int32_t channels);

/**
 * Get the number of active channels.
 *
 * This is an internal function to allow the atomic update of both the
 * number of active channels and the sample rate.
 *
 * When the number of channels changes, use the new value.  Otherwise,
 * return the curent value.
 *
 * @return Value to write to the active channels field.
 */
int32_t getAnalogNumChannelsToActivate(int32_t* status);

/**
 * Set the sample rate.
 *
 * This is a global setting for the Athena and effects all channels.
 *
 * @param samplesPerSecond The number of samples per channel per second.
 */
void setAnalogSampleRate(double samplesPerSecond, int32_t* status);

}  // namespace hal
