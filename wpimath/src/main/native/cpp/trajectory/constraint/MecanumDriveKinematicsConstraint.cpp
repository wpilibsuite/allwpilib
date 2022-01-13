// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/constraint/MecanumDriveKinematicsConstraint.h"

#include "units/math.h"

using namespace frc;

MecanumDriveKinematicsConstraint::MecanumDriveKinematicsConstraint(
    const MecanumDriveKinematics& kinematics,
    units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

units::meters_per_second_t MecanumDriveKinematicsConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  auto xVelocity = velocity * pose.Rotation().Cos();
  auto yVelocity = velocity * pose.Rotation().Sin();
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({xVelocity, yVelocity, velocity * curvature});
  wheelSpeeds.Desaturate(m_maxSpeed);

  auto normSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);

  return units::math::hypot(normSpeeds.vx, normSpeeds.vy);
}

TrajectoryConstraint::MinMax
MecanumDriveKinematicsConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  return {};
}
