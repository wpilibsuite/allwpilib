/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
  wheelSpeeds.Normalize(m_maxSpeed);

  auto normSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);

  return units::math::hypot(normSpeeds.vx, normSpeeds.vy);
}

TrajectoryConstraint::MinMax
MecanumDriveKinematicsConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  return {};
}
