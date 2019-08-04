/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWM.h"

#include <utility>

#include <hal/HAL.h>
#include <hal/PWM.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"

using namespace frc;

PWM::PWM(int channel) {
  if (!SensorUtil::CheckPWMChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "PWM Channel " + wpi::Twine(channel));
    return;
  }

  int32_t status = 0;
  m_handle = HAL_InitializePWMPort(HAL_GetPort(channel), &status);
  if (status != 0) {
    wpi_setErrorWithContextRange(status, 0, HAL_GetNumPWMChannels(), channel,
                                 HAL_GetErrorMessage(status));
    m_channel = std::numeric_limits<int>::max();
    m_handle = HAL_kInvalidHandle;
    return;
  }

  m_channel = channel;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, false, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  HAL_Report(HALUsageReporting::kResourceType_PWM, channel);
  SetName("PWM", channel);

  SetSafetyEnabled(false);
}

PWM::~PWM() {
  int32_t status = 0;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  HAL_FreePWMPort(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

PWM::PWM(PWM&& rhs)
    : MotorSafety(std::move(rhs)),
      SendableBase(std::move(rhs)),
      m_channel(std::move(rhs.m_channel)) {
  std::swap(m_handle, rhs.m_handle);
}

PWM& PWM::operator=(PWM&& rhs) {
  ErrorBase::operator=(std::move(rhs));
  SendableBase::operator=(std::move(rhs));

  m_channel = std::move(rhs.m_channel);
  std::swap(m_handle, rhs.m_handle);

  return *this;
}

void PWM::StopMotor() { SetDisabled(); }

void PWM::GetDescription(wpi::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

void PWM::SetRaw(uint16_t value) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetPWMRaw(m_handle, value, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

uint16_t PWM::GetRaw() const {
  if (StatusIsFatal()) return 0;

  int32_t status = 0;
  uint16_t value = HAL_GetPWMRaw(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  return value;
}

void PWM::SetPosition(double pos) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMPosition(m_handle, pos, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

double PWM::GetPosition() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double position = HAL_GetPWMPosition(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return position;
}

void PWM::SetSpeed(double speed) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMSpeed(m_handle, speed, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));

  Feed();
}

double PWM::GetSpeed() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double speed = HAL_GetPWMSpeed(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return speed;
}

void PWM::SetDisabled() {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::SetPeriodMultiplier(PeriodMultiplier mult) {
  if (StatusIsFatal()) return;

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
      wpi_assert(false);
  }

  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::SetZeroLatch() {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  HAL_LatchPWMZero(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::EnableDeadbandElimination(bool eliminateDeadband) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, eliminateDeadband, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::SetBounds(double max, double deadbandMax, double center,
                    double deadbandMin, double min) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMConfig(m_handle, max, deadbandMax, center, deadbandMin, min,
                   &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::SetRawBounds(int max, int deadbandMax, int center, int deadbandMin,
                       int min) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMConfigRaw(m_handle, max, deadbandMax, center, deadbandMin, min,
                      &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void PWM::GetRawBounds(int* max, int* deadbandMax, int* center,
                       int* deadbandMin, int* min) {
  int32_t status = 0;
  HAL_GetPWMConfigRaw(m_handle, max, deadbandMax, center, deadbandMin, min,
                      &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

int PWM::GetChannel() const { return m_channel; }

void PWM::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("PWM");
  builder.SetActuator(true);
  builder.SetSafeState([=]() { SetDisabled(); });
  builder.AddDoubleProperty("Value", [=]() { return GetRaw(); },
                            [=](double value) { SetRaw(value); });
}
