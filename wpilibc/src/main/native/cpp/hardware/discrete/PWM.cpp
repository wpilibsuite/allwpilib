// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hardware/discrete/PWM.hpp"

#include <utility>

#include "wpi/hal/HALBase.h"
#include "wpi/hal/PWM.h"
#include "wpi/hal/Ports.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/system/Errors.hpp"
#include "wpi/util/SensorUtil.hpp"
#include "wpi/util/StackTrace.hpp"
#include "wpi/util/sendable/SendableBuilder.hpp"
#include "wpi/util/sendable/SendableRegistry.hpp"

using namespace wpi;

PWM::PWM(int channel, bool registerSendable) {
  if (!SensorUtil::CheckPWMChannel(channel)) {
    throw WPILIB_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  auto stack = wpi::util::GetStackTrace(1);
  int32_t status = 0;
  m_handle = HAL_InitializePWMPort(channel, stack.c_str(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", channel);

  m_channel = channel;

  SetDisabled();

  HAL_ReportUsage("IO", channel, "PWM");
  if (registerSendable) {
    wpi::util::SendableRegistry::Add(this, "PWM", channel);
  }
}

PWM::~PWM() {
  if (m_handle != HAL_kInvalidHandle) {
    SetDisabled();
  }
}

void PWM::SetPulseTime(wpi::units::microsecond_t time) {
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds(m_handle, time.value(), &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

wpi::units::microsecond_t PWM::GetPulseTime() const {
  int32_t status = 0;
  double value = HAL_GetPWMPulseTimeMicroseconds(m_handle, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);

  return wpi::units::microsecond_t{value};
}

void PWM::SetDisabled() {
  int32_t status = 0;
  HAL_SetPWMPulseTimeMicroseconds(m_handle, 0, &status);
  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
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
      throw WPILIB_MakeError(err::InvalidParameter, "OutputPeriod value {}",
                             static_cast<int>(mult));
  }

  WPILIB_CheckErrorStatus(status, "Channel {}", m_channel);
}

int PWM::GetChannel() const {
  return m_channel;
}

void PWM::SetSimDevice(HAL_SimDeviceHandle device) {
  HAL_SetPWMSimDevice(m_handle, device);
}

void PWM::InitSendable(wpi::util::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PWM");
  builder.SetActuator(true);
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetPulseTime().value(); },
      [=, this](double value) {
        SetPulseTime(wpi::units::microsecond_t{value});
      });
}
