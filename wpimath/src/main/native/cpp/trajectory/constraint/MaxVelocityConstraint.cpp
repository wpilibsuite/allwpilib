// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/constraint/MaxVelocityConstraint.h"

using namespace frc;

MaxVelocityConstraint::MaxVelocityConstraint(
    units::meters_per_second_t maxVelocity)
    : m_maxVelocity(units::math::abs(maxVelocity)) {}

units::meters_per_second_t MaxVelocityConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  return m_maxVelocity;
}

TrajectoryConstraint::MinMax MaxVelocityConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  return {};
}
