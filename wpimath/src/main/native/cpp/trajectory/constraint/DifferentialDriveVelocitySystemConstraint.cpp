/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/DifferentialDriveVelocitySystemConstraint.h"

#include <algorithm>
#include <limits>

#include <wpi/MathExtras.h>

#include "units/velocity.h"

using namespace frc;

DifferentialDriveVelocitySystemConstraint::
    DifferentialDriveVelocitySystemConstraint(
        const LinearSystem<2, 2, 2>& system,
        const DifferentialDriveKinematics& kinematics, units::volt_t maxVoltage)
    : m_system(system), m_kinematics(kinematics), m_maxVoltage(maxVoltage) {}

units::meters_per_second_t
DifferentialDriveVelocitySystemConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  // Calculate wheel velocity states from current velocity and curvature
  auto [vl, vr] =
      m_kinematics.ToWheelSpeeds({velocity, 0_mps, velocity * curvature});

  Eigen::Vector2d x;
  x << vl.to<double>(), vr.to<double>();

  // If either wheel velocity is greater than its maximum, normalize the wheel
  // speeds to within an achievable range while maintaining the curvature
  if (std::abs(x(0)) > velocity.to<double>() ||
      std::abs(x(1)) > velocity.to<double>()) {
    x *= velocity.to<double>() / x.lpNorm<Eigen::Infinity>();
  }
  return units::meters_per_second_t{(x(0) + x(1)) / 2.0};
}

TrajectoryConstraint::MinMax
DifferentialDriveVelocitySystemConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({speed, 0_mps, speed * curvature});

  Eigen::Vector2d x;
  x << wheelSpeeds.left.to<double>(), wheelSpeeds.right.to<double>();

  Eigen::Vector2d xDot;
  Eigen::Vector2d u;

  // dx/dt for minimum u
  u << -m_maxVoltage.to<double>(), -m_maxVoltage.to<double>();
  xDot = m_system.A() * x + m_system.B() * u;
  units::meters_per_second_squared_t minChassisAcceleration;
  minChassisAcceleration =
      units::meters_per_second_squared_t((xDot(0, 0) + xDot(1, 0)) / 2.0);

  // dx/dt for maximum u
  u << m_maxVoltage.to<double>(), m_maxVoltage.to<double>();
  xDot = m_system.A() * x + m_system.B() * u;
  units::meters_per_second_squared_t maxChassisAcceleration;
  maxChassisAcceleration =
      units::meters_per_second_squared_t((xDot(0, 0) + xDot(1, 0)) / 2.0);

  return {minChassisAcceleration, maxChassisAcceleration};
}
