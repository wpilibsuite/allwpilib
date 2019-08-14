/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/PWMBase.h"

#include <utility>

#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <hal/PWM.h>
#include <hal/Ports.h>

#include "frc/SensorUtil.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

using namespace frc;

PWMBase::PWMBase(int channel) {
  if (!SensorUtil::CheckPWMChannel(channel)) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange,
                               "PWM Channel " + wpi::Twine(channel));
    return;
  }

  int32_t status = 0;
  m_handle = HAL_InitializePWMPort(HAL_GetPort(channel), &status);
  if (status != 0) {
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumPWMChannels(), channel);
    m_channel = std::numeric_limits<int>::max();
    m_handle = HAL_kInvalidHandle;
    return;
  }

  m_channel = channel;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setHALError(status);
  status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, false, &status);
  wpi_setHALError(status);

  HAL_Report(HALUsageReporting::kResourceType_PWM, channel + 1);

  SetSafetyEnabled(false);
}

PWMBase::~PWMBase() {
  int32_t status = 0;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setHALError(status);

  HAL_FreePWMPort(m_handle, &status);
  wpi_setHALError(status);
}

void PWMBase::StopMotor() { SetDisabled(); }

void PWMBase::GetDescription(wpi::raw_ostream& desc) const {
  desc << "PWM " << GetChannel();
}

void PWMBase::SetRaw(uint16_t value) {
  if (StatusIsFatal()) return;

  int32_t status = 0;
  HAL_SetPWMRaw(m_handle, value, &status);
  wpi_setHALError(status);
}

uint16_t PWMBase::GetRaw() const {
  if (StatusIsFatal()) return 0;

  int32_t status = 0;
  uint16_t value = HAL_GetPWMRaw(m_handle, &status);
  wpi_setHALError(status);

  return value;
}

void PWMBase::SetPosition(double pos) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMPosition(m_handle, pos, &status);
  wpi_setHALError(status);
}

double PWMBase::GetPosition() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double position = HAL_GetPWMPosition(m_handle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return position;
}

void PWMBase::SetSpeed(double speed) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMSpeed(m_handle, speed, &status);
  wpi_setHALError(status);

  Feed();
}

double PWMBase::GetSpeed() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double speed = HAL_GetPWMSpeed(m_handle, &status);
  wpi_setHALError(status);
  return speed;
}

void PWMBase::SetDisabled() {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  HAL_SetPWMDisabled(m_handle, &status);
  wpi_setHALError(status);
}

void PWMBase::SetPeriodMultiplier(PeriodMultiplier mult) {
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

  wpi_setHALError(status);
}

void PWMBase::SetZeroLatch() {
  if (StatusIsFatal()) return;

  int32_t status = 0;

  HAL_LatchPWMZero(m_handle, &status);
  wpi_setHALError(status);
}

void PWMBase::EnableDeadbandElimination(bool eliminateDeadband) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMEliminateDeadband(m_handle, eliminateDeadband, &status);
  wpi_setHALError(status);
}

void PWMBase::SetBounds(double max, double deadbandMax, double center,
                        double deadbandMin, double min) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMConfig(m_handle, max, deadbandMax, center, deadbandMin, min,
                   &status);
  wpi_setHALError(status);
}

void PWMBase::SetRawBounds(int max, int deadbandMax, int center,
                           int deadbandMin, int min) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetPWMConfigRaw(m_handle, max, deadbandMax, center, deadbandMin, min,
                      &status);
  wpi_setHALError(status);
}

void PWMBase::GetRawBounds(int* max, int* deadbandMax, int* center,
                           int* deadbandMin, int* min) {
  int32_t status = 0;
  HAL_GetPWMConfigRaw(m_handle, max, deadbandMax, center, deadbandMin, min,
                      &status);
  wpi_setHALError(status);
}

int PWMBase::GetChannel() const { return m_channel; }
