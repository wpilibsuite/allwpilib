// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h"

using namespace frc;

DifferentialDriveKinematicsConstraint::DifferentialDriveKinematicsConstraint(
    const DifferentialDriveKinematics& kinematics,
    units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

units::meters_per_second_t DifferentialDriveKinematicsConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({velocity, 0_mps, velocity * curvature});
  wheelSpeeds.Desaturate(m_maxSpeed);

  return m_kinematics.ToChassisSpeeds(wheelSpeeds).vx;
}

TrajectoryConstraint::MinMax
DifferentialDriveKinematicsConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  return {};
}
