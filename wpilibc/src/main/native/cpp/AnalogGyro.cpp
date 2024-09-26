// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/AnalogGyro.h"

#include <climits>
#include <memory>
#include <string>

#include <hal/AnalogGyro.h>
#include <hal/Errors.h>
#include <hal/FRCUsageReporting.h>
#include <wpi/NullDeleter.h>
#include <wpi/StackTrace.h>
#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "frc/AnalogInput.h"
#include "frc/Errors.h"

using namespace frc;

AnalogGyro::AnalogGyro(int channel)
    : AnalogGyro(std::make_shared<AnalogInput>(channel)) {
  wpi::SendableRegistry::AddChild(this, m_analog.get());
}

AnalogGyro::AnalogGyro(AnalogInput* channel)
    : AnalogGyro(std::shared_ptr<AnalogInput>(
          channel, wpi::NullDeleter<AnalogInput>())) {}

AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel)
    : m_analog(channel) {
  if (!channel) {
    throw FRC_MakeError(err::NullParameter, "channel");
  }
  InitGyro();
  Calibrate();
}

AnalogGyro::AnalogGyro(int channel, int center, double offset)
    : AnalogGyro(std::make_shared<AnalogInput>(channel), center, offset) {
  wpi::SendableRegistry::AddChild(this, m_analog.get());
}

AnalogGyro::AnalogGyro(std::shared_ptr<AnalogInput> channel, int center,
                       double offset)
    : m_analog(channel) {
  if (!channel) {
    throw FRC_MakeError(err::NullParameter, "channel");
  }
  InitGyro();
  int32_t status = 0;
  HAL_SetAnalogGyroParameters(m_gyroHandle, kDefaultVoltsPerDegreePerSecond,
                              offset, center, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  Reset();
}

double AnalogGyro::GetAngle() const {
  int32_t status = 0;
  double value = HAL_GetAnalogGyroAngle(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  return value;
}

double AnalogGyro::GetRate() const {
  int32_t status = 0;
  double value = HAL_GetAnalogGyroRate(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  return value;
}

int AnalogGyro::GetCenter() const {
  int32_t status = 0;
  int value = HAL_GetAnalogGyroCenter(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  return value;
}

double AnalogGyro::GetOffset() const {
  int32_t status = 0;
  double value = HAL_GetAnalogGyroOffset(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  return value;
}

void AnalogGyro::SetSensitivity(double voltsPerDegreePerSecond) {
  int32_t status = 0;
  HAL_SetAnalogGyroVoltsPerDegreePerSecond(m_gyroHandle,
                                           voltsPerDegreePerSecond, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
}

void AnalogGyro::SetDeadband(double volts) {
  int32_t status = 0;
  HAL_SetAnalogGyroDeadband(m_gyroHandle, volts, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
}

void AnalogGyro::Reset() {
  int32_t status = 0;
  HAL_ResetAnalogGyro(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
}

void AnalogGyro::InitGyro() {
  if (m_gyroHandle == HAL_kInvalidHandle) {
    int32_t status = 0;
    std::string stackTrace = wpi::GetStackTrace(1);
    m_gyroHandle =
        HAL_InitializeAnalogGyro(m_analog->m_port, stackTrace.c_str(), &status);
    FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
  }

  int32_t status = 0;
  HAL_SetupAnalogGyro(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());

  HAL_Report(HALUsageReporting::kResourceType_Gyro, m_analog->GetChannel() + 1);

  wpi::SendableRegistry::AddLW(this, "AnalogGyro", m_analog->GetChannel());
}

void AnalogGyro::Calibrate() {
  int32_t status = 0;
  HAL_CalibrateAnalogGyro(m_gyroHandle, &status);
  FRC_CheckErrorStatus(status, "Channel {}", m_analog->GetChannel());
}

Rotation2d AnalogGyro::GetRotation2d() const {
  return units::degree_t{-GetAngle()};
}

std::shared_ptr<AnalogInput> AnalogGyro::GetAnalogInput() const {
  return m_analog;
}

void AnalogGyro::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("Gyro");
  builder.AddDoubleProperty("Value", [=, this] { return GetAngle(); }, nullptr);
}
