// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/SensorUtil.h"

#include <hal/AnalogInput.h>
#include <hal/AnalogOutput.h>
#include <hal/DIO.h>
#include <hal/PWM.h>
#include <hal/Ports.h>
#include <hal/Relay.h>

using namespace frc;

const int SensorUtil::kDigitalChannels = HAL_GetNumDigitalChannels();
const int SensorUtil::kAnalogInputs = HAL_GetNumAnalogInputs();
const int SensorUtil::kAnalogOutputs = HAL_GetNumAnalogOutputs();
const int SensorUtil::kPwmChannels = HAL_GetNumPWMChannels();
const int SensorUtil::kRelayChannels = HAL_GetNumRelayHeaders();

int SensorUtil::GetDefaultCTREPCMModule() {
  return 0;
}

int SensorUtil::GetDefaultREVPHModule() {
  return 1;
}

bool SensorUtil::CheckDigitalChannel(int channel) {
  return HAL_CheckDIOChannel(channel);
}

bool SensorUtil::CheckRelayChannel(int channel) {
  return HAL_CheckRelayChannel(channel);
}

bool SensorUtil::CheckPWMChannel(int channel) {
  return HAL_CheckPWMChannel(channel);
}

bool SensorUtil::CheckAnalogInputChannel(int channel) {
  return HAL_CheckAnalogInputChannel(channel);
}

bool SensorUtil::CheckAnalogOutputChannel(int channel) {
  return HAL_CheckAnalogOutputChannel(channel);
}
