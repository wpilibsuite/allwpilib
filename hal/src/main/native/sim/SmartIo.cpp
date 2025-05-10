// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "SmartIo.h"

namespace hal {
DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo>* smartIoHandles;

namespace init {
void InitializeSmartIo() {
  static DigitalHandleResource<HAL_DigitalHandle, SmartIo, kNumSmartIo> dcH;
  smartIoHandles = &dcH;
}
}  // namespace init
}
