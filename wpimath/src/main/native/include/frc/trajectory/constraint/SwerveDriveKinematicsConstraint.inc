// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/trajectory/constraint/SwerveDriveKinematicsConstraint.h"
#include "units/math.h"

namespace frc {

template <size_t NumModules>

/**
 * A class that enforces constraints on the swerve drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities of the wheels stay below a certain limit.
 */
SwerveDriveKinematicsConstraint<NumModules>::SwerveDriveKinematicsConstraint(
    const frc::SwerveDriveKinematics<NumModules>& kinematics,
    units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

template <size_t NumModules>
units::meters_per_second_t
SwerveDriveKinematicsConstraint<NumModules>::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  auto xVelocity = velocity * pose.Rotation().Cos();
  auto yVelocity = velocity * pose.Rotation().Sin();
  auto wheelSpeeds = m_kinematics.ToSwerveModuleStates(
      {xVelocity, yVelocity, velocity * curvature});
  m_kinematics.NormalizeWheelSpeeds(&wheelSpeeds, m_maxSpeed);

  auto normSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);

  return units::math::hypot(normSpeeds.vx, normSpeeds.vy);
}

template <size_t NumModules>
TrajectoryConstraint::MinMax
SwerveDriveKinematicsConstraint<NumModules>::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  return {};
}

}  // namespace frc
