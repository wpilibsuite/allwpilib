/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h"

using namespace frc;

DifferentialDriveKinematicsConstraint::DifferentialDriveKinematicsConstraint(
    DifferentialDriveKinematics kinematics, units::meters_per_second_t maxSpeed)
    : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

units::meters_per_second_t DifferentialDriveKinematicsConstraint::MaxVelocity(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t velocity) {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({velocity, 0_mps, velocity * curvature});
  wheelSpeeds.Normalize(m_maxSpeed);

  return m_kinematics.ToChassisSpeeds(wheelSpeeds).vx;
}

TrajectoryConstraint::MinMax
DifferentialDriveKinematicsConstraint::MinMaxAcceleration(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t speed) {
  return {};
}
