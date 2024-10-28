// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/PWM.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/PWM.h>
#include <hal/Ports.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/Errors.h"
#include "frc/SensorUtil.h"

using namespace frc;

PWM::PWM(int channel, bool registerSendable) {
  if (!SensorUtil::CheckPWMChannel(channel)) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "Channel {}", channel);
  }

  auto stack = wpi::GetStackTrace(1);
  int32_t status = 0;
  m_handle =
      HAL_InitializePWMPort(HAL_GetPort(channel), stack.c_str(), &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  m_channel = channel;

  HAL_SetPWMDisabled(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);
  status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, false, &status);
  FRC_CheckErrorStatus(status, "Channel {}", channel);

  HAL_Report(HALUsageReporting::kResourceType_PWM, channel + 1);
  if (registerSendable) {
    wpi::SendableRegistry::AddLW(this, "PWM", channel);
  }
}

PWM::~PWM() {
  if (m_handle != HAL_kInvalidHandle) {
    int32_t status = 0;
    HAL_SetPWMDisabled(m_handle, &status);
    FRC_ReportError(status, "Channel {}", m_channel);
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

void PWM::SetPosition(double pos) {
  int32_t status = 0;
  HAL_SetPWMPosition(m_handle, pos, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

double PWM::GetPosition() const {
  int32_t status = 0;
  double position = HAL_GetPWMPosition(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return position;
}

void PWM::SetSpeed(double speed) {
  int32_t status = 0;
  HAL_SetPWMSpeed(m_handle, speed, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

double PWM::GetSpeed() const {
  int32_t status = 0;
  double speed = HAL_GetPWMSpeed(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
  return speed;
}

void PWM::SetDisabled() {
  int32_t status = 0;
  HAL_SetPWMDisabled(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::SetPeriodMultiplier(PeriodMultiplier mult) {
  int32_t status = 0;

  switch (mult) {
    case kPeriodMultiplier_4X:
      HAL_SetPWMPeriodScale(m_handle, 3,
                            &status);  // Squelch 3 out of 4 outputs
      break;
    case kPeriodMultiplier_2X:
      HAL_SetPWMPeriodScale(m_handle, 1,
                            &status);  // Squelch 1 out of 2 outputs
      break;
    case kPeriodMultiplier_1X:
      HAL_SetPWMPeriodScale(m_handle, 0, &status);  // Don't squelch any outputs
      break;
    default:
      throw FRC_MakeError(err::InvalidParameter, "PeriodMultiplier value {}",
                          static_cast<int>(mult));
  }

  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::SetZeroLatch() {
  int32_t status = 0;
  HAL_LatchPWMZero(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::EnableDeadbandElimination(bool eliminateDeadband) {
  int32_t status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, eliminateDeadband, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::SetBounds(units::microsecond_t max, units::microsecond_t deadbandMax,
                    units::microsecond_t center,
                    units::microsecond_t deadbandMin,
                    units::microsecond_t min) {
  int32_t status = 0;
  HAL_SetPWMConfigMicroseconds(m_handle, max.value(), deadbandMax.value(),
                               center.value(), deadbandMin.value(), min.value(),
                               &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::GetBounds(units::microsecond_t* max,
                    units::microsecond_t* deadbandMax,
                    units::microsecond_t* center,
                    units::microsecond_t* deadbandMin,
                    units::microsecond_t* min) {
  int32_t status = 0;
  int32_t rawMax, rawDeadbandMax, rawCenter, rawDeadbandMin, rawMin;
  HAL_GetPWMConfigMicroseconds(m_handle, &rawMax, &rawDeadbandMax, &rawCenter,
                               &rawDeadbandMin, &rawMin, &status);
  *max = units::microsecond_t{static_cast<double>(rawMax)};
  *deadbandMax = units::microsecond_t{static_cast<double>(rawDeadbandMax)};
  *center = units::microsecond_t{static_cast<double>(rawCenter)};
  *deadbandMin = units::microsecond_t{static_cast<double>(rawDeadbandMin)};
  *min = units::microsecond_t{static_cast<double>(rawMin)};
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

void PWM::SetAlwaysHighMode() {
  int32_t status = 0;
  HAL_SetPWMAlwaysHighMode(m_handle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_channel);
}

int PWM::GetChannel() const {
  return m_channel;
}

void PWM::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("PWM");
  builder.SetActuator(true);
  builder.SetSafeState([=, this] { SetDisabled(); });
  builder.AddDoubleProperty(
      "Value", [=, this] { return GetPulseTime().value(); },
      [=, this](double value) { SetPulseTime(units::millisecond_t{value}); });
  builder.AddDoubleProperty(
      "Speed", [=, this] { return GetSpeed(); },
      [=, this](double value) { SetSpeed(value); });
  builder.AddDoubleProperty(
      "Position", [=, this] { return GetPosition(); },
      [=, this](double value) { SetPosition(value); });
}
