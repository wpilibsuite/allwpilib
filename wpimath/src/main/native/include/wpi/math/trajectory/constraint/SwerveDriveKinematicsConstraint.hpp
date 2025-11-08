// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>

#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/math/trajectory/constraint/TrajectoryConstraint.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"

namespace wpi::math {

/**
 * A class that enforces constraints on the swerve drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities of the wheels stay below a certain limit.
 */
template <size_t NumModules>
class SwerveDriveKinematicsConstraint : public TrajectoryConstraint {
 public:
  SwerveDriveKinematicsConstraint(
      const wpi::math::SwerveDriveKinematics<NumModules>& kinematics,
      wpi::units::meters_per_second_t maxSpeed)
      : m_kinematics(kinematics), m_maxSpeed(maxSpeed) {}

  wpi::units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t velocity) const override {
    auto xVelocity = velocity * pose.Rotation().Cos();
    auto yVelocity = velocity * pose.Rotation().Sin();
    auto wheelSpeeds = m_kinematics.ToSwerveModuleStates(
        {xVelocity, yVelocity, velocity * curvature});
    m_kinematics.DesaturateWheelSpeeds(&wheelSpeeds, m_maxSpeed);

    auto normSpeeds = m_kinematics.ToChassisSpeeds(wheelSpeeds);

    return wpi::units::math::hypot(normSpeeds.vx, normSpeeds.vy);
  }

  MinMax MinMaxAcceleration(
      const Pose2d& pose, wpi::units::curvature_t curvature,
      wpi::units::meters_per_second_t speed) const override {
    return {};
  }

 private:
  wpi::math::SwerveDriveKinematics<NumModules> m_kinematics;
  wpi::units::meters_per_second_t m_maxSpeed;
};

}  // namespace wpi::math
