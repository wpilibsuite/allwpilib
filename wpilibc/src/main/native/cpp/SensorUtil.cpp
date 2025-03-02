// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SensorUtil.h"

#include <hal/AnalogInput.h>
#include <hal/DIO.h>
#include <hal/PWM.h>
#include <hal/Ports.h>

using namespace frc;

int SensorUtil::GetDefaultCTREPCMModule() {
  return 0;
}

int SensorUtil::GetDefaultREVPHModule() {
  return 1;
}

int SensorUtil::GetNumSmartIoPorts() {
  return HAL_GetNumSmartIo();
}
