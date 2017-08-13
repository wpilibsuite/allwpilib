/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>

#include "HAL/AnalogTrigger.h"
#include "HAL/ChipObject.h"
#include "HAL/Ports.h"
#include "HAL/Types.h"
#include "HAL/handles/DigitalHandleResource.h"
#include "HAL/handles/HandlesInternal.h"
#include "PortsInternal.h"

namespace hal {
/**
 * MXP channels when used as digital output PWM are offset from actual value
 */
constexpr int32_t kMXPDigitalPWMOffset = 6;

constexpr int32_t kExpectedLoopTiming = 40;

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
constexpr double kDefaultPwmPeriod = 5.05;
/**
 * kDefaultPwmCenter is the PWM range center in ms
 */
constexpr double kDefaultPwmCenter = 1.5;
/**
 * kDefaultPWMStepsDown is the number of PWM steps below the centerpoint
 */
constexpr int32_t kDefaultPwmStepsDown = 1000;
constexpr int32_t kPwmDisabled = 0;

// Create a mutex to protect changes to the DO PWM config
extern priority_recursive_mutex digitalPwmMutex;

extern std::unique_ptr<tDIO> digitalSystem;
extern std::unique_ptr<tRelay> relaySystem;
extern std::unique_ptr<tPWM> pwmSystem;
extern std::unique_ptr<tSPI> spiSystem;

struct DigitalPort {
  uint8_t channel;
  bool configSet = false;
  bool eliminateDeadband = false;
  int32_t maxPwm = 0;
  int32_t deadbandMaxPwm = 0;
  int32_t centerPwm = 0;
  int32_t deadbandMinPwm = 0;
  int32_t minPwm = 0;
};

extern DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                             kNumDigitalChannels + kNumPWMHeaders>
    digitalChannelHandles;

void initializeDigital(int32_t* status);
bool remapDigitalSource(HAL_Handle digitalSourceHandle,
                        HAL_AnalogTriggerType analogTriggerType,
                        uint8_t& channel, uint8_t& module, bool& analogTrigger);
int32_t remapSPIChannel(int32_t channel);
int32_t remapMXPPWMChannel(int32_t channel);
int32_t remapMXPChannel(int32_t channel);
}  // namespace hal
