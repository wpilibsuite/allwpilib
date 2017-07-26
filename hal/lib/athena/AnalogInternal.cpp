/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInternal.h"

#include <atomic>

#include "HAL/AnalogInput.h"
#include "HAL/ChipObject.h"
#include "HAL/cpp/priority_mutex.h"
#include "PortsInternal.h"

hal::priority_recursive_mutex hal::analogRegisterWindowMutex;
std::unique_ptr<hal::tAI> hal::analogInputSystem;
std::unique_ptr<hal::tAO> hal::analogOutputSystem;

hal::IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                           hal::kNumAnalogInputs,
                           hal::HAL_HandleEnum::AnalogInput>
    hal::analogInputHandles;

static int32_t analogNumChannelsToActivate = 0;

static std::atomic<bool> analogSystemInitialized{false};

bool hal::analogSampleRateSet = false;

/**
 * Initialize the analog System.
 */
void hal::initializeAnalog(int32_t* status) {
  if (analogSystemInitialized) return;
  std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
  if (analogSystemInitialized) return;
  analogInputSystem.reset(tAI::create(status));
  analogOutputSystem.reset(tAO::create(status));
  setAnalogNumChannelsToActivate(kNumAnalogInputs);
  setAnalogSampleRate(kDefaultSampleRate, status);
  analogSystemInitialized = true;
}

/**
 * Return the number of channels on the module in use.
 *
 * @return Active channels.
 */
int32_t hal::getAnalogNumActiveChannels(int32_t* status) {
  int32_t scanSize = analogInputSystem->readConfig_ScanSize(status);
  if (scanSize == 0) return 8;
  return scanSize;
}

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
int32_t hal::getAnalogNumChannelsToActivate(int32_t* status) {
  if (analogNumChannelsToActivate == 0)
    return getAnalogNumActiveChannels(status);
  return analogNumChannelsToActivate;
}

/**
 * Set the sample rate.
 *
 * This is a global setting for the Athena and effects all channels.
 *
 * @param samplesPerSecond The number of samples per channel per second.
 */
void hal::setAnalogSampleRate(double samplesPerSecond, int32_t* status) {
  // TODO: This will change when variable size scan lists are implemented.
  // TODO: Need double comparison with epsilon.
  // wpi_assert(!sampleRateSet || GetSampleRate() == samplesPerSecond);
  analogSampleRateSet = true;

  // Compute the convert rate
  uint32_t ticksPerSample =
      static_cast<uint32_t>(static_cast<double>(kTimebase) / samplesPerSecond);
  uint32_t ticksPerConversion =
      ticksPerSample / getAnalogNumChannelsToActivate(status);
  // ticksPerConversion must be at least 80
  if (ticksPerConversion < 80) {
    if ((*status) >= 0) *status = SAMPLE_RATE_TOO_HIGH;
    ticksPerConversion = 80;
  }

  // Atomically set the scan size and the convert rate so that the sample rate
  // is constant
  tAI::tConfig config;
  config.ScanSize = getAnalogNumChannelsToActivate(status);
  config.ConvertRate = ticksPerConversion;
  analogInputSystem->writeConfig(config, status);

  // Indicate that the scan size has been commited to hardware.
  setAnalogNumChannelsToActivate(0);
}

/**
 * Set the number of active channels.
 *
 * Store the number of active channels to set.  Don't actually commit to
 * hardware
 * until SetSampleRate().
 *
 * @param channels Number of active channels.
 */
void hal::setAnalogNumChannelsToActivate(int32_t channels) {
  analogNumChannelsToActivate = channels;
}
