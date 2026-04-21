// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/SensorUtil.hpp"

#include "wpi/hal/Ports.h"

using namespace wpi;

int SensorUtil::GetDefaultCTREPCMModule() {
  return 0;
}

int SensorUtil::GetDefaultREVPHModule() {
  return 1;
}

int SensorUtil::GetNumSmartIoPorts() {
  return HAL_GetNumSmartIo();
}
