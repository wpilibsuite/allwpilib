// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <string>

#include "PortsInternal.hpp"
#include "wpi/hal/handles/DigitalHandleResource.hpp"

namespace wpi::hal {

struct DigitalPort {
  uint8_t channel;
  std::string previousAllocation;
};

extern DigitalHandleResource<HAL_DigitalHandle, DigitalPort,
                             NUM_DIGITAL_CHANNELS + NUM_PWM_HEADERS>*
    digitalChannelHandles;

int32_t GetDigitalInputChannel(HAL_DigitalHandle handle, int32_t* status);
}  // namespace wpi::hal
