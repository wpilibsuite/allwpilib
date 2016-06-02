/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DigitalInternal.h"

#include <mutex>
#include <thread>

#include "ChipObject.h"
#include "FRC_NetworkCommunication/LoadOut.h"
#include "HAL/HAL.h"
#include "HAL/cpp/Resource.h"
#include "HAL/cpp/priority_mutex.h"

static_assert(sizeof(uint32_t) <= sizeof(void*),
              "This file shoves uint32_ts into pointers.");
namespace hal {
// Create a mutex to protect changes to the DO PWM config
priority_recursive_mutex digitalPwmMutex;

tDIO* digitalSystem = nullptr;
tRelay* relaySystem = nullptr;
tPWM* pwmSystem = nullptr;
hal::Resource* DIOChannels = nullptr;
hal::Resource* DO_PWMGenerators = nullptr;
hal::Resource* PWMChannels = nullptr;

bool digitalSystemsInitialized = false;

/**
 * Initialize the digital system.
 */
void initializeDigital(int32_t* status) {
  if (digitalSystemsInitialized) return;

  hal::Resource::CreateResourceObject(&DIOChannels,
                                      tDIO::kNumSystems * kDigitalPins);
  hal::Resource::CreateResourceObject(
      &DO_PWMGenerators,
      tDIO::kNumPWMDutyCycleAElements + tDIO::kNumPWMDutyCycleBElements);
  hal::Resource::CreateResourceObject(&PWMChannels,
                                      tPWM::kNumSystems * kPwmPins);
  digitalSystem = tDIO::create(status);

  // Relay Setup
  relaySystem = tRelay::create(status);

  // Turn off all relay outputs.
  relaySystem->writeValue_Forward(0, status);
  relaySystem->writeValue_Reverse(0, status);

  // PWM Setup
  pwmSystem = tPWM::create(status);

  // Make sure that the 9403 IONode has had a chance to initialize before
  // continuing.
  while (pwmSystem->readLoopTiming(status) == 0) std::this_thread::yield();

  if (pwmSystem->readLoopTiming(status) != kExpectedLoopTiming) {
    *status = LOOP_TIMING_ERROR;  // NOTE: Doesn't display the error
  }

  // Calculate the length, in ms, of one DIO loop
  double loopTime = pwmSystem->readLoopTiming(status) /
                    (kSystemClockTicksPerMicrosecond * 1e3);

  pwmSystem->writeConfig_Period((uint16_t)(kDefaultPwmPeriod / loopTime + .5),
                                status);
  uint16_t minHigh = (uint16_t)(
      (kDefaultPwmCenter - kDefaultPwmStepsDown * loopTime) / loopTime + .5);
  pwmSystem->writeConfig_MinHigh(minHigh, status);
  // Ensure that PWM output values are set to OFF
  for (uint32_t pwm_index = 0; pwm_index < kPwmPins; pwm_index++) {
    // Initialize port structure
    DigitalPort digital_port;
    digital_port.pin = pwm_index;

    setPWM(&digital_port, kPwmDisabled, status);
    setPWMPeriodScale(&digital_port, 3, status);  // Set all to 4x by default.
  }

  digitalSystemsInitialized = true;
}

/**
 * Map DIO pin numbers from their physical number (10 to 26) to their position
 * in the bit field.
 */
uint32_t remapMXPChannel(uint32_t pin) { return pin - 10; }

uint32_t remapMXPPWMChannel(uint32_t pin) {
  if (pin < 14) {
    return pin - 10;  // first block of 4 pwms (MXP 0-3)
  } else {
    return pin - 6;  // block of PWMs after SPI
  }
}

/**
 * remap the digital source pin and set the module.
 * If it's an analog trigger, determine the module from the high order routing
 * channel else do normal digital input remapping based on pin number (MXP)
 */
extern "C++" void remapDigitalSource(bool analogTrigger, uint32_t& pin,
                                     uint8_t& module) {
  if (analogTrigger) {
    module = pin >> 4;
  } else {
    if (pin >= kNumHeaders) {
      pin = remapMXPChannel(pin);
      module = 1;
    } else {
      module = 0;
    }
  }
}
}
