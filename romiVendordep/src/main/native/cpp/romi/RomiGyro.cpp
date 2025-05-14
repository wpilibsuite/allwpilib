// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/romi/RomiGyro.h>

using namespace frc;

RomiGyro::RomiGyro() : m_simDevice("Gyro:RomiGyro") {
  if (m_simDevice) {
    m_simDevice.CreateBoolean("init", hal::SimDevice::OUTPUT, true);
    m_simRateX = m_simDevice.CreateDouble("rate_x", hal::SimDevice::INPUT, 0.0);
    m_simRateY = m_simDevice.CreateDouble("rate_y", hal::SimDevice::INPUT, 0.0);
    m_simRateZ = m_simDevice.CreateDouble("rate_z", hal::SimDevice::INPUT, 0.0);
    m_simAngleX =
        m_simDevice.CreateDouble("angle_x", hal::SimDevice::INPUT, 0.0);
    m_simAngleY =
        m_simDevice.CreateDouble("angle_y", hal::SimDevice::INPUT, 0.0);
    m_simAngleZ =
        m_simDevice.CreateDouble("angle_z", hal::SimDevice::INPUT, 0.0);
  }
}

units::radian_t RomiGyro::GetAngle() const {
  return GetAngleZ();
}

units::radians_per_second_t RomiGyro::GetRate() const {
  return GetRateZ();
}

units::radians_per_second_t RomiGyro::GetRateX() const {
  if (m_simRateX) {
    return units::degrees_per_second_t{m_simRateX.Get()};
  }

  return 0.0_rad_per_s;
}

units::radians_per_second_t RomiGyro::GetRateY() const {
  if (m_simRateY) {
    return units::degrees_per_second_t{m_simRateY.Get()};
  }

  return 0.0_rad_per_s;
}

units::radians_per_second_t RomiGyro::GetRateZ() const {
  if (m_simRateZ) {
    return units::degrees_per_second_t{m_simRateZ.Get()};
  }

  return 0.0_rad_per_s;
}

units::radian_t RomiGyro::GetAngleX() const {
  if (m_simAngleX) {
    return units::degree_t{m_simAngleX.Get() - m_angleXOffset};
  }

  return 0.0_rad;
}

units::radian_t RomiGyro::GetAngleY() const {
  if (m_simAngleY) {
    return units::degree_t{m_simAngleY.Get() - m_angleYOffset};
  }

  return 0.0_rad;
}

units::radian_t RomiGyro::GetAngleZ() const {
  if (m_simAngleZ) {
    return units::degree_t{m_simAngleZ.Get() - m_angleZOffset};
  }

  return 0.0_rad;
}

void RomiGyro::Reset() {
  if (m_simAngleX) {
    m_angleXOffset = m_simAngleX.Get();
    m_angleYOffset = m_simAngleY.Get();
    m_angleZOffset = m_simAngleZ.Get();
  }
}
