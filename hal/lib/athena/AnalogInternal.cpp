/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "AnalogInternal.h"

#include "ChipObject.h"
#include "HAL/AnalogInput.h"
#include "HAL/cpp/priority_mutex.h"

namespace hal {
priority_recursive_mutex analogRegisterWindowMutex;
tAI* analogInputSystem = nullptr;
tAO* analogOutputSystem = nullptr;
static uint32_t analogNumChannelsToActivate = 0;

bool analogSystemInitialized = false;

/**
 * Initialize the analog System.
 */
void initializeAnalog(int32_t* status) {
  std::lock_guard<priority_recursive_mutex> sync(analogRegisterWindowMutex);
  if (analogSystemInitialized) return;
  analogInputSystem = tAI::create(status);
  analogOutputSystem = tAO::create(status);
  setAnalogNumChannelsToActivate(kAnalogInputPins);
  setAnalogSampleRate(kDefaultSampleRate, status);
  analogSystemInitialized = true;
}

/**
 * Return the number of channels on the module in use.
 *
 * @return Active channels.
 */
uint32_t getAnalogNumActiveChannels(int32_t* status) {
  uint32_t scanSize = analogInputSystem->readConfig_ScanSize(status);
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
uint32_t getAnalogNumChannelsToActivate(int32_t* status) {
  if (analogNumChannelsToActivate == 0)
    return getAnalogNumActiveChannels(status);
  return analogNumChannelsToActivate;
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
void setAnalogNumChannelsToActivate(uint32_t channels) {
  analogNumChannelsToActivate = channels;
}
}
