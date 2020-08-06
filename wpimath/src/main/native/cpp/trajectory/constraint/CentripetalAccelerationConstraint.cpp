/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/CentripetalAccelerationConstraint.h"

#include "units/math.h"

using namespace frc;

CentripetalAccelerationConstraint::CentripetalAccelerationConstraint(
    units::meters_per_second_squared_t maxCentripetalAcceleration)
    : m_maxCentripetalAcceleration(maxCentripetalAcceleration) {}

units::meters_per_second_t CentripetalAccelerationConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  // ac = v^2 / r
  // k (curvature) = 1 / r

  // therefore, ac = v^2 * k
  // ac / k = v^2
  // v = std::sqrt(ac / k)

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
