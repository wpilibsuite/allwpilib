// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>
#include <string>

#include "PortsInternal.h"
#include "hal/handles/DigitalHandleResource.h"
#include "hal/handles/HandlesInternal.h"

namespace hal {
struct SmartIo {
  uint8_t channel;
  std::string previousAllocation;
};

extern DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>*
    smartIoHandles;
}  // namespace hal
