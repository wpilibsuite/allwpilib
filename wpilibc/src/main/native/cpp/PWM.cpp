// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PWM.h"

#include <hal/HALBase.h>
#include <hal/PWM.h>
#include <hal/Ports.h>
#include <hal/UsageReporting.h>
#include <wpi/StackTrace.h>
#include <wpi/telemetry/TelemetryTable.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

PWM::PWM(int channel) {
  if (!SensorUtil::CheckPWMChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  auto stack = wpi::GetStackTrace(1);
  int32_t status = 0;
  m_handle = HAL_InitializePWMPort(channel, stack.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  m_channel = channel;

  SetDisabled();

  HAL_ReportUsage("IO", channel, "PWM");
}

PWM::~PWM() {
  if (m_handle != HAL_kInvalidHandle) {
    SetDisabled();
  }
}

void PWM::SetPulseTime(units::microsecond_t time) {
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds(m_handle, time.value(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

units::microsecond_t PWM::GetPulseTime() const {
  int32_t status = 0;
  double value = HAL_GetPWMPulseTimeMicroseconds(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);

  return units::microsecond_t{value};
}

void PWM::SetDisabled() {
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds(m_handle, 0, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::SetOutputPeriod(OutputPeriod mult) {
  int32_t status = 0;

  switch (mult) {
    case kOutputPeriod_20Ms:
      HAL_SetPWMOutputPeriod(m_handle, 3,
                             &status);  // Squelch 3 out of 4 outputs
      break;
    case kOutputPeriod_10Ms:
      HAL_SetPWMOutputPeriod(m_handle, 1,
                             &status);  // Squelch 1 out of 2 outputs
      break;
    case kOutputPeriod_5Ms:
      HAL_SetPWMOutputPeriod(m_handle, 0,
                             &status);  // Don't squelch any outputs
      break;
    default:
      throw FRC_MakeError(err::InvalidParameter, "OutputPeriod value {}",
                          static_cast<int>(mult));
  }

  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

int PWM::GetChannel() const {
  return m_channel;
}

void PWM::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetPWMSimDevice(m_handle, device);
}

void PWM::UpdateTelemetry(wpi::TelemetryTable& table) const {
  table.Log("Value", GetPulseTime().value());
}

std::string_view PWM::GetTelemetryType() const {
  return "PWM";
}
