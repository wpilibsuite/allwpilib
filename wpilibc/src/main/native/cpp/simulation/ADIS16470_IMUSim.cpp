// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADIS16470_IMUSim.h"

#include <frc/ADIS16470_IMU.h>
#include <frc/simulation/SimDeviceSim.h>

using namespace frc::sim;

ADIS16470_IMUSim::ADIS16470_IMUSim(const frc::ADIS16470_IMU& imu) {
  frc::sim::SimDeviceSim deviceSim{"Gyro:ADIS16470", imu.GetPort()};
  m_simGyroAngleX = deviceSim.GetDouble("gyro_angle_x");
  m_simGyroAngleY = deviceSim.GetDouble("gyro_angle_y");
  m_simGyroAngleZ = deviceSim.GetDouble("gyro_angle_z");
  m_simGyroRateX = deviceSim.GetDouble("gyro_rate_x");
  m_simGyroRateY = deviceSim.GetDouble("gyro_rate_y");
  m_simGyroRateZ = deviceSim.GetDouble("gyro_rate_z");
  m_simAccelX = deviceSim.GetDouble("accel_x");
  m_simAccelY = deviceSim.GetDouble("accel_y");
  m_simAccelZ = deviceSim.GetDouble("accel_z");
}

void ADIS16470_IMUSim::SetGyroAngleX(units::degree_t angle) {
  m_simGyroAngleX.Set(angle.value());
}

void ADIS16470_IMUSim::SetGyroAngleY(units::degree_t angle) {
  m_simGyroAngleY.Set(angle.value());
}

void ADIS16470_IMUSim::SetGyroAngleZ(units::degree_t angle) {
  m_simGyroAngleZ.Set(angle.value());
}

void ADIS16470_IMUSim::SetGyroRateX(units::degrees_per_second_t angularRate) {
  m_simGyroRateX.Set(angularRate.value());
}

void ADIS16470_IMUSim::SetGyroRateY(units::degrees_per_second_t angularRate) {
  m_simGyroRateY.Set(angularRate.value());
}

void ADIS16470_IMUSim::SetGyroRateZ(units::degrees_per_second_t angularRate) {
  m_simGyroRateZ.Set(angularRate.value());
}

void ADIS16470_IMUSim::SetAccelX(units::meters_per_second_squared_t accel) {
  m_simAccelX.Set(accel.value());
}

void ADIS16470_IMUSim::SetAccelY(units::meters_per_second_squared_t accel) {
  m_simAccelY.Set(accel.value());
}

void ADIS16470_IMUSim::SetAccelZ(units::meters_per_second_squared_t accel) {
  m_simAccelZ.Set(accel.value());
}
