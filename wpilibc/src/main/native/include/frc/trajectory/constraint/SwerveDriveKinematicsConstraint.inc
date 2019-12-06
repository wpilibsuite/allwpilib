/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

/**
 * A class that enforces constraints on the differential drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities for both sides of the drivetrain stay below a certain
 * limit.
 */

namespace frc {

template <size_t NumModules>
SwerveDriveKinematicsConstraint<NumModules>::SwerveDriveKinematicsConstraint(
    frc::SwerveDriveKinematics<NumModules> kinematics,
    units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

template <size_t NumModules>
units::meters_per_second_t
SwerveDriveKinematicsConstraint<NumModules>::MaxVelocity(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t velocity) {
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
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t speed) {
  return {};
}

}  // namespace frc
