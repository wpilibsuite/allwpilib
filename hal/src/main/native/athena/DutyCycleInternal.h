// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include "hal/ChipObject.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"

namespace hal {
struct DutyCycle {
  std::unique_ptr<tDutyCycle> dutyCycle;
  int index;
};

extern LimitedHandleResource<HAL_DutyCycleHandle, DutyCycle, kNumDutyCycles,
                             HAL_HandleEnum::DutyCycle>* dutyCycleHandles;
}  // namespace hal
