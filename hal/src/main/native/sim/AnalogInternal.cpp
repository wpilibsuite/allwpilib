// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "AnalogInternal.h"

#include "PortsInternal.h"
#include "hal/handles/IndexedHandleResource.h"

namespace hal {
IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort, kNumAnalogInputs,
                      HAL_HandleEnum::AnalogInput>* analogInputHandles;
}  // namespace hal

namespace hal::init {
void InitializeAnalogInternal() {
  static IndexedHandleResource<HAL_AnalogInputHandle, hal::AnalogPort,
                               kNumAnalogInputs, HAL_HandleEnum::AnalogInput>
      aiH;
  analogInputHandles = &aiH;
}
}  // namespace hal::init
