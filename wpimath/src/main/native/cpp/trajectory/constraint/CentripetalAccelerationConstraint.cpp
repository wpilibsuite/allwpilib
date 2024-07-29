// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/constraint/CentripetalAccelerationConstraint.h"

#include "units/math.h"

using namespace frc;

CentripetalAccelerationConstraint::CentripetalAccelerationConstraint(
    units::meters_per_second_squared_t maxCentripetalAcceleration)
    : m_maxCentripetalAcceleration(maxCentripetalAcceleration) {}

units::meters_per_second_t CentripetalAccelerationConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  // ac = v²/r
  // k (curvature) = 1/r

  // therefore, ac = v²k
  // ac/k = v²
  // v = √(ac/k)

  // We have to multiply by 1_rad here to get the units to cancel out nicely.
  // The units library defines a unit for radians although it is technically
  // unitless.
  return units::math::sqrt(m_maxCentripetalAcceleration /
                           units::math::abs(curvature) * 1_rad);
}

TrajectoryConstraint::MinMax
CentripetalAccelerationConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  // The acceleration of the robot has no impact on the centripetal acceleration
  // of the robot.
  return {};
}
