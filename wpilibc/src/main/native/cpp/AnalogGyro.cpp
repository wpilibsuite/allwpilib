/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/AnalogGyro.h"

#include <climits>
#include <utility>

#include <hal/AnalogGyro.h>
#include <hal/Errors.h>
#include <hal/HAL.h>

#include "frc/AnalogInput.h"
#include "frc/Timer.h"
#include "frc/WPIErrors.h"

using namespace frc;

AnalogGyro::AnalogGyro(int channel)
    : AnalogGyro(std::make_shared<AnalogInput>(channel)) {
  AddChild(m_analog);
}

AnalogGyro::AnalogGyro(AnalogInput* channel)
    : AnalogGyro(
          std::shared_ptr<AnalogInput>(channel, NullDeleter<AnalogInput>())) {}

AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel)
    : m_analog(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitGyro();
    Calibrate();
  }
}

AnalogGyro::AnalogGyro(int channel, int center, double offset)
    : AnalogGyro(std::make_shared<AnalogInput>(channel), center, offset) {
  AddChild(m_analog);
}

AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel, int center,
                       double offset)
    : m_analog(channel) {
  if (channel == nullptr) {
    wpi_setWPIError(NullParameter);
  } else {
    InitGyro();
    int32_t status = 0;
    HAL_SetAnalogGyroParameters(m_gyroHandle, kDefaultVoltsPerDegreePerSecond,
                                offset, center, &status);
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
    Reset();
  }
}

AnalogGyro::~AnalogGyro() { HAL_FreeAnalogGyro(m_gyroHandle); }

AnalogGyro::AnalogGyro(AnalogGyro&& rhs)
    : GyroBase(std::move(rhs)), m_analog(std::move(rhs.m_analog)) {
  std::swap(m_gyroHandle, rhs.m_gyroHandle);
}

AnalogGyro& AnalogGyro::operator=(AnalogGyro&& rhs) {
  GyroBase::operator=(std::move(rhs));

  m_analog = std::move(rhs.m_analog);
  std::swap(m_gyroHandle, rhs.m_gyroHandle);

  return *this;
}

double AnalogGyro::GetAngle() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroAngle(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

double AnalogGyro::GetRate() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroRate(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

int AnalogGyro::GetCenter() const {
  if (StatusIsFatal()) return 0;
  int32_t status = 0;
  int value = HAL_GetAnalogGyroCenter(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

double AnalogGyro::GetOffset() const {
  if (StatusIsFatal()) return 0.0;
  int32_t status = 0;
  double value = HAL_GetAnalogGyroOffset(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return value;
}

void AnalogGyro::SetSensitivity(double voltsPerDegreePerSecond) {
  int32_t status = 0;
  HAL_SetAnalogGyroVoltsPerDegreePerSecond(m_gyroHandle,
                                           voltsPerDegreePerSecond, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogGyro::SetDeadband(double volts) {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_SetAnalogGyroDeadband(m_gyroHandle, volts, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogGyro::Reset() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_ResetAnalogGyro(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}

void AnalogGyro::InitGyro() {
  if (StatusIsFatal()) return;
  if (m_gyroHandle == HAL_kInvalidHandle) {
    int32_t status = 0;
    m_gyroHandle = HAL_InitializeAnalogGyro(m_analog->m_port, &status);
    if (status == PARAMETER_OUT_OF_RANGE) {
      wpi_setWPIErrorWithContext(ParameterOutOfRange,
                                 " channel (must be accumulator channel)");
      m_analog = nullptr;
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
    if (status != 0) {
      wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
      m_analog = nullptr;
      m_gyroHandle = HAL_kInvalidHandle;
      return;
    }
  }

  int32_t status = 0;
  HAL_SetupAnalogGyro(m_gyroHandle, &status);
  if (status != 0) {
    wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
    m_analog = nullptr;
    m_gyroHandle = HAL_kInvalidHandle;
    return;
  }

  HAL_Report(HALUsageReporting::kResourceType_Gyro, m_analog->GetChannel());
  SetName("AnalogGyro", m_analog->GetChannel());
}

void AnalogGyro::Calibrate() {
  if (StatusIsFatal()) return;
  int32_t status = 0;
  HAL_CalibrateAnalogGyro(m_gyroHandle, &status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
}
