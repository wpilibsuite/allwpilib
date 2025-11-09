// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogInternal.h"

#include "PortsInternal.h"
#include "wpi/hal/handles/IndexedHandleResource.h"

namespace wpi::hal {
IndexedHandleResource<HAL_AnalogInputHandle, wpi::hal::AnalogPort,
                      kNumAnalogInputs, HAL_HandleEnum::AnalogInput>*
    analogInputHandles;
}  // namespace wpi::hal

namespace wpi::hal::init {
void InitializeAnalogInternal() {
  static IndexedHandleResource<HAL_AnalogInputHandle, wpi::hal::AnalogPort,
                               kNumAnalogInputs, HAL_HandleEnum::AnalogInput>
      aiH;
  analogInputHandles = &aiH;
}
}  // namespace wpi::hal::init
