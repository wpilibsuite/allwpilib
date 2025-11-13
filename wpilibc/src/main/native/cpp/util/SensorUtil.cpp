// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/SensorUtil.hpp"

#include "wpi/hal/AnalogInput.h"
#include "wpi/hal/DIO.h"
#include "wpi/hal/PWM.h"
#include "wpi/hal/Ports.h"

using namespace wpi;

int SensorUtil::GetDefaultCTREPCMModule() {
  return 0;
}

int SensorUtil::GetDefaultREVPHModule() {
  return 1;
}

bool SensorUtil::CheckDigitalChannel(int channel) {
  return HAL_CheckDIOChannel(channel);
}

bool SensorUtil::CheckPWMChannel(int channel) {
  return HAL_CheckPWMChannel(channel);
}

bool SensorUtil::CheckAnalogInputChannel(int channel) {
  return HAL_CheckAnalogInputChannel(channel);
}

int SensorUtil::GetNumDigitalChannels() {
  return HAL_GetNumDigitalChannels();
}

int SensorUtil::GetNumAnalogInputs() {
  return HAL_GetNumAnalogInputs();
}

int SensorUtil::GetNumPwmChannels() {
  return HAL_GetNumPWMChannels();
}
