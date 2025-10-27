// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include <wpi/SymbolExports.h>

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/velocity.h"

namespace frc {
/**
 * A class that enforces constraints on the differential drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities for both sides of the drivetrain stay below a certain
 * limit.
 */
class WPILIB_DLLEXPORT DifferentialDriveKinematicsConstraint
    : public TrajectoryConstraint {
 public:
  constexpr DifferentialDriveKinematicsConstraint(
      DifferentialDriveKinematics kinematics,
      units::meters_per_second_t maxSpeed)
      : m_kinematics(std::move(kinematics)), m_maxSpeed(maxSpeed) {}

  constexpr units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override {
    auto wheelSpeeds =
        m_kinematics.ToWheelSpeeds({velocity, 0_mps, velocity * curvature});
    wheelSpeeds.Desaturate(m_maxSpeed);

    return m_kinematics.ToChassisSpeeds(wheelSpeeds).vx;
  }

  constexpr MinMax MinMaxAcceleration(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t speed) const override {
    return {};
  }

 private:
  DifferentialDriveKinematics m_kinematics;
  units::meters_per_second_t m_maxSpeed;
};
}  // namespace frc
