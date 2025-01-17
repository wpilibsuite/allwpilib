// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/SystemServer.h"

extern "C" {

NT_Inst HAL_GetSystemServerHandle(void) {
  return 0;
}
}  // extern "C"
