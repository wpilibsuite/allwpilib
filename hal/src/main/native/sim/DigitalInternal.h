// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include "PortsInternal.h"
#include "wpi/hal/handles/DigitalHandleResource.h"

namespace wpi::hal {
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
 * - 5ms periods allows higher update rates for modern motor controllers.
 *
 * kDefaultPwmPeriod is the 1x period (5.05 ms).  In hardware, the period
 * scaling is implemented as an output squelch to get longer periods for old
 * devices.
 */
constexpr float kDefaultPwmPeriod = 5.05f;
constexpr int32_t kPwmDisabled = 0;

struct DigitalPort {
  uint8_t channel;
  int32_t filterIndex = 0;
  std::string previousAllocation;
};

extern DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                             kNumDigitalChannels + kNumPWMHeaders>*
    digitalChannelHandles;

/**
 * Map DIO channel numbers from their physical number (10 to 26) to their
 * position in the bit field.
 */
int32_t remapMXPChannel(int32_t channel);

int32_t remapMXPPWMChannel(int32_t channel);

int32_t GetDigitalInputChannel(HAL_DigitalHandle handle, int32_t* status);
}  // namespace wpi::hal
