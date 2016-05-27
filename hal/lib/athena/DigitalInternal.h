/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include "ChipObject.h"
#include "HAL/cpp/Resource.h"

namespace hal {
constexpr uint32_t kNumHeaders = 10;  // Number of non-MXP pins
constexpr uint32_t kDigitalPins = 26;
constexpr uint32_t kPwmPins = 20;
constexpr uint32_t kMXPDigitalPWMOffset = 6;  // MXP pins when used as digital
                                              // output pwm are offset by 6 from
                                              // actual value
constexpr uint32_t kExpectedLoopTiming = 40;

/**
 * kDefaultPwmPeriod is in ms
 *
 * - 20ms periods (50 Hz) are the "safest" setting in that this works for all
 *   devices
 * - 20ms periods seem to be desirable for Vex Motors
 * - 20ms periods are the specified period for HS-322HD servos, but work
 *   reliably down to 10.0 ms; starting at about 8.5ms, the servo sometimes hums
 *   and get hot; by 5.0ms the hum is nearly continuous
 * - 10ms periods work well for Victor 884
 * - 5ms periods allows higher update rates for Luminary Micro Jaguar speed
 *   controllers. Due to the shipping firmware on the Jaguar, we can't run the
 *   update period less than 5.05 ms.
 *
 * kDefaultPwmPeriod is the 1x period (5.05 ms).  In hardware, the period
 * scaling is implemented as an output squelch to get longer periods for old
 * devices.
 */
constexpr float kDefaultPwmPeriod = 5.05;
/**
 * kDefaultPwmCenter is the PWM range center in ms
 */
constexpr float kDefaultPwmCenter = 1.5;
/**
 * kDefaultPWMStepsDown is the number of PWM steps below the centerpoint
 */
constexpr int32_t kDefaultPwmStepsDown = 1000;
constexpr int32_t kPwmDisabled = 0;

// Create a mutex to protect changes to the DO PWM config
extern priority_recursive_mutex digitalPwmMutex;

extern tDIO* digitalSystem;
extern tRelay* relaySystem;
extern tPWM* pwmSystem;
extern hal::Resource* DIOChannels;
extern hal::Resource* DO_PWMGenerators;
extern hal::Resource* PWMChannels;

extern bool digitalSystemsInitialized;

struct DigitalPort {
  uint8_t pin;
  uint32_t PWMGeneratorID;
};

void initializeDigital(int32_t* status);
void remapDigitalSource(bool analogTrigger, uint32_t& pin, uint8_t& module);
uint32_t remapMXPPWMChannel(uint32_t pin);
uint32_t remapMXPChannel(uint32_t pin);
}
