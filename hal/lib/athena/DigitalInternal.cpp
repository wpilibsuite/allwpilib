/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInternal.h"

#include <atomic>
#include <mutex>
#include <thread>

#include "ConstantsInternal.h"
#include "FRC_NetworkCommunication/LoadOut.h"
#include "HAL/AnalogTrigger.h"
#include "HAL/ChipObject.h"
#include "HAL/HAL.h"
#include "HAL/Ports.h"
#include "HAL/cpp/priority_mutex.h"
#include "PortsInternal.h"

namespace hal {
// Create a mutex to protect changes to the DO PWM config
priority_recursive_mutex digitalPwmMutex;

std::unique_ptr<tDIO> digitalSystem;
std::unique_ptr<tRelay> relaySystem;
std::unique_ptr<tPWM> pwmSystem;
std::unique_ptr<tSPI> spiSystem;

static std::atomic<bool> digitalSystemsInitialized{false};
static hal::priority_mutex initializeMutex;

DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                      kNumDigitalChannels + kNumPWMHeaders>
    digitalChannelHandles;

/**
 * Initialize the digital system.
 */
void initializeDigital(int32_t* status) {
  // Initial check, as if it's true initialization has finished
  if (digitalSystemsInitialized) return;

  std::lock_guard<hal::priority_mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (digitalSystemsInitialized) return;

  digitalSystem.reset(tDIO::create(status));

  // Relay Setup
  relaySystem.reset(tRelay::create(status));

  // Turn off all relay outputs.
  relaySystem->writeValue_Forward(0, status);
  relaySystem->writeValue_Reverse(0, status);

  // PWM Setup
  pwmSystem.reset(tPWM::create(status));

  // Make sure that the 9403 IONode has had a chance to initialize before
  // continuing.
  while (pwmSystem->readLoopTiming(status) == 0) std::this_thread::yield();

  if (pwmSystem->readLoopTiming(status) != kExpectedLoopTiming) {
    *status = LOOP_TIMING_ERROR;  // NOTE: Doesn't display the error
  }

  // Calculate the length, in ms, of one DIO loop
  double loopTime = pwmSystem->readLoopTiming(status) /
                    (kSystemClockTicksPerMicrosecond * 1e3);

  pwmSystem->writeConfig_Period(
      static_cast<uint16_t>(kDefaultPwmPeriod / loopTime + .5), status);
  uint16_t minHigh = static_cast<uint16_t>(
      (kDefaultPwmCenter - kDefaultPwmStepsDown * loopTime) / loopTime + .5);
  pwmSystem->writeConfig_MinHigh(minHigh, status);
  // Ensure that PWM output values are set to OFF
  for (uint8_t pwmIndex = 0; pwmIndex < kNumPWMChannels; pwmIndex++) {
    // Copy of SetPWM
    if (pwmIndex < tPWM::kNumHdrRegisters) {
      pwmSystem->writeHdr(pwmIndex, kPwmDisabled, status);
    } else {
      pwmSystem->writeMXP(pwmIndex - tPWM::kNumHdrRegisters, kPwmDisabled,
                          status);
    }

    // Copy of SetPWMPeriodScale, set to 4x by default.
    if (pwmIndex < tPWM::kNumPeriodScaleHdrElements) {
      pwmSystem->writePeriodScaleHdr(pwmIndex, 3, status);
    } else {
      pwmSystem->writePeriodScaleMXP(
          pwmIndex - tPWM::kNumPeriodScaleHdrElements, 3, status);
    }
  }

  // SPI setup
  spiSystem.reset(tSPI::create(status));

  digitalSystemsInitialized = true;
}

/**
 * Map SPI channel numbers from their physical number (27 to 31) to their
 * position in the bit field.
 */
int32_t remapSPIChannel(int32_t channel) { return channel - 26; }

/**
 * Map DIO channel numbers from their physical number (10 to 26) to their
 * position in the bit field.
 */
int32_t remapMXPChannel(int32_t channel) { return channel - 10; }

int32_t remapMXPPWMChannel(int32_t channel) {
  if (channel < 14) {
    return channel - 10;  // first block of 4 pwms (MXP 0-3)
  } else {
    return channel - 6;  // block of PWMs after SPI
  }
}

/**
 * remap the digital source channel and set the module.
 * If it's an analog trigger, determine the module from the high order routing
 * channel else do normal digital input remapping based on channel number
 * (MXP)
 */
bool remapDigitalSource(HAL_Handle digitalSourceHandle,
                        HAL_AnalogTriggerType analogTriggerType,
                        uint8_t& channel, uint8_t& module,
                        bool& analogTrigger) {
  if (isHandleType(digitalSourceHandle, HAL_HandleEnum::AnalogTrigger)) {
    // If handle passed, index is not negative
    int32_t index = getHandleIndex(digitalSourceHandle);
    channel = (index << 2) + analogTriggerType;
    module = channel >> 4;
    analogTrigger = true;
    return true;
  } else if (isHandleType(digitalSourceHandle, HAL_HandleEnum::DIO)) {
    int32_t index = getHandleIndex(digitalSourceHandle);
    if (index > kNumDigitalHeaders + kNumDigitalMXPChannels) {
      // channels 10-15, so need to add headers to remap index
      channel = remapSPIChannel(index) + kNumDigitalHeaders;
      module = 0;
    } else if (index >= kNumDigitalHeaders) {
      channel = remapMXPChannel(index);
      module = 1;
    } else {
      channel = index;
      module = 0;
    }
    analogTrigger = false;
    return true;
  } else {
    return false;
  }
}
}  // namespace hal
