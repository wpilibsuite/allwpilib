// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/xrp/XRPGyro.h"

#include <units/angle.h>
#include <units/angular_velocity.h>

using namespace frc;

XRPGyro::XRPGyro() : m_simDevice("Gyro:XRPGyro") {
  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::kOutput, true);
    m_simRateX =
        m_simDevice.CreateDouble("rate_x", hal::SimDevice::kInput, 0.0);
    m_simRateY =
        m_simDevice.CreateDouble("rate_y", hal::SimDevice::kInput, 0.0);
    m_simRateZ =
        m_simDevice.CreateDouble("rate_z", hal::SimDevice::kInput, 0.0);
    m_simAngleX =
        m_simDevice.CreateDouble("angle_x", hal::SimDevice::kInput, 0.0);
    m_simAngleY =
        m_simDevice.CreateDouble("angle_y", hal::SimDevice::kInput, 0.0);
    m_simAngleZ =
        m_simDevice.CreateDouble("angle_z", hal::SimDevice::kInput, 0.0);
  }
}

units::radian_t XRPGyro::GetAngle() const {
  return GetAngleZ();
}

frc::Rotation2d XRPGyro::GetRotation2d() const {
  return frc::Rotation2d{GetAngle()};
}

units::radians_per_second_t XRPGyro::GetRate() const {
  return GetRateZ();
}

units::radians_per_second_t XRPGyro::GetRateX() const {
  if (m_simRateX) {
    return units::degrees_per_second_t{m_simRateX.Get()};
  }

  return 0_rad_per_s;
}

units::radians_per_second_t XRPGyro::GetRateY() const {
  if (m_simRateY) {
    return units::degrees_per_second_t{m_simRateY.Get()};
  }

  return 0_rad_per_s;
}

units::radians_per_second_t XRPGyro::GetRateZ() const {
  if (m_simRateZ) {
    return units::degrees_per_second_t{m_simRateZ.Get()};
  }

  return 0_rad_per_s;
}

units::radian_t XRPGyro::GetAngleX() const {
  if (m_simAngleX) {
    return units::degree_t{m_simAngleX.Get()} - m_angleXOffset;
  }

  return 0_rad;
}

units::radian_t XRPGyro::GetAngleY() const {
  if (m_simAngleY) {
    return units::degree_t{m_simAngleY.Get()} - m_angleYOffset;
  }

  return 0_rad;
}

units::radian_t XRPGyro::GetAngleZ() const {
  if (m_simAngleZ) {
    return units::degree_t{m_simAngleZ.Get()} - m_angleZOffset;
  }

  return 0_rad;
}

void XRPGyro::Reset() {
  if (m_simAngleX) {
    m_angleXOffset = units::degree_t{m_simAngleX.Get()};
    m_angleYOffset = units::degree_t{m_simAngleY.Get()};
    m_angleZOffset = units::degree_t{m_simAngleZ.Get()};
  }
}
