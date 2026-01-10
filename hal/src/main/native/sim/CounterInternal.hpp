// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "PortsInternal.hpp"
#include "wpi/hal/handles/HandlesInternal.hpp"
#include "wpi/hal/handles/LimitedHandleResource.hpp"

namespace wpi::hal {

struct Counter {
  uint8_t index;
};

extern LimitedHandleResource<HAL_CounterHandle, Counter, kNumCounters,
                             HAL_HandleEnum::Counter>* counterHandles;

}  // namespace wpi::hal
