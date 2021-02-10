// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

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
class DifferentialDriveKinematicsConstraint : public TrajectoryConstraint {
 public:
  DifferentialDriveKinematicsConstraint(
      const DifferentialDriveKinematics& kinematics,
      units::meters_per_second_t maxSpeed);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override;

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override;

 private:
  const DifferentialDriveKinematics& m_kinematics;
  units::meters_per_second_t m_maxSpeed;
};
}  // namespace frc
