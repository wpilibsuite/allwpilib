/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HAL/handles/HandlesInternal.h"
#include "HAL/handles/LimitedHandleResource.h"
#include "PortsInternal.h"

namespace hal {

struct Counter {
  uint8_t index;
};

extern LimitedHandleResource<HAL_CounterHandle, Counter, kNumCounters,
                             HAL_HandleEnum::Counter>* counterHandles;

}  // namespace hal
