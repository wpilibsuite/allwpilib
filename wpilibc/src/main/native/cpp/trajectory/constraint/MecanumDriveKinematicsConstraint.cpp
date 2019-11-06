/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/MecanumDriveKinematicsConstraint.h"

using namespace frc;

MecanumDriveKinematicsConstraint::MecanumDriveKinematicsConstraint(
    MecanumDriveKinematics kinematics, units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

units::meters_per_second_t MecanumDriveKinematicsConstraint::MaxVelocity(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t velocity) {
  auto xVelocity = velocity * sin(pose.Rotation().Radians().to<double>());
  auto yVelocity = velocity * cos(pose.Rotation().Radians().to<double>());
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({xVelocity, yVelocity, velocity * curvature});
  wheelSpeeds.Normalize(m_maxSpeed);

  return m_kinematics.ToChassisSpeeds(wheelSpeeds).vx;
}

TrajectoryConstraint::MinMax
MecanumDriveKinematicsConstraint::MinMaxAcceleration(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t speed) {
  return {};
}