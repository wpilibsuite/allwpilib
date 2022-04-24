// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/DifferentialDriveAccelerationLimiter.h"

#include <utility>

#include "Eigen/QR"

using namespace frc;

DifferentialDriveAccelerationLimiter::DifferentialDriveAccelerationLimiter(
    LinearSystem<2, 2, 2> system, units::meter_t trackwidth,
    units::meters_per_second_squared_t maxLinearAccel,
    units::radians_per_second_squared_t maxAngularAccel)
    : m_system{std::move(system)},
      m_trackwidth{trackwidth},
      m_maxLinearAccel{maxLinearAccel},
      m_maxAngularAccel{maxAngularAccel} {}

DifferentialDriveAccelerationLimiter::WheelVoltages
DifferentialDriveAccelerationLimiter::Calculate(
    units::meters_per_second_t leftVelocity,
    units::meters_per_second_t rightVelocity, units::volt_t leftVoltage,
    units::volt_t rightVoltage) {
  Eigen::Vector<double, 2> u{leftVoltage.value(), rightVoltage.value()};

  // Find unconstrained wheel accelerations
  Eigen::Vector<double, 2> x{leftVelocity.value(), rightVelocity.value()};
  Eigen::Vector<double, 2> dxdt = m_system.A() * x + m_system.B() * u;

  // Converts from wheel accelerations to linear and angular acceleration
  // a = (dxdt(0) + dxdt(1)) / 2.0
  // alpha = (dxdt(1) - dxdt(0)) / trackwidth
  Eigen::Matrix<double, 2, 2> M{
      {0.5, 0.5}, {-1.0 / m_trackwidth.value(), 1.0 / m_trackwidth.value()}};

  // Convert to linear and angular accelerations, constrain them, then convert
  // back
  Eigen::Vector<double, 2> accels = M * dxdt;
  if (accels(0) > m_maxLinearAccel.value()) {
    accels(0) = m_maxLinearAccel.value();
  } else if (accels(0) < -m_maxLinearAccel.value()) {
    accels(0) = -m_maxLinearAccel.value();
  }
  if (accels(1) > m_maxAngularAccel.value()) {
    accels(1) = m_maxAngularAccel.value();
  } else if (accels(1) < -m_maxAngularAccel.value()) {
    accels(1) = -m_maxAngularAccel.value();
  }
  dxdt = M.householderQr().solve(accels);

  // Find voltages for the given wheel accelerations
  // dx/dt = Ax + Bu
  // u = B⁻¹(dx/dt - Ax)
  u = m_system.B().householderQr().solve(dxdt - m_system.A() * x);

  return {units::volt_t{u(0)}, units::volt_t{u(1)}};
}
