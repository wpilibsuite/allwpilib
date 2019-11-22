/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cmath>

#include <units/units.h>

#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

/**
 * A class that enforces constraints on the differential drive kinematics.
 * This can be used to ensure that the trajectory is constructed so that the
 * commanded velocities for both sides of the drivetrain stay below a certain
 * limit.
 */
namespace frc {

template <size_t NumModules>
class SwerveDriveKinematicsConstraint : public TrajectoryConstraint {
 public:
  SwerveDriveKinematicsConstraint(
      frc::SwerveDriveKinematics<NumModules> kinematics,
      units::meters_per_second_t maxSpeed);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, curvature_t curvature,
      units::meters_per_second_t velocity) override;

  MinMax MinMaxAcceleration(const Pose2d& pose, curvature_t curvature,
                            units::meters_per_second_t speed) override;

 private:
  frc::SwerveDriveKinematics<NumModules> m_kinematics;
  units::meters_per_second_t m_maxSpeed;
};
}  // namespace frc

#include "SwerveDriveKinematicsConstraint.inc"
