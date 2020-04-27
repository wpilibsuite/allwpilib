/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/system/LinearSystem.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/voltage.h"

namespace frc {
/**
 * A class that enforces constraints on differential drive velocity based on
 * a differential drive {@link LinearSystem} and the drive kinematics. Ensures
 * that the acceleration of any wheel of the robot while following the
 * trajectory is never higher than what can be achieved with the given maximum
 * voltage.
 */
class DifferentialDriveVelocitySystemConstraint : public TrajectoryConstraint {
 public:
  /**
   * Creates a new DifferentialDriveVelocitySystemConstraint.
   *
   * @param system      A {@link LinearSystem} representing the drivetrain..
   * @param kinematics  A kinematics component describing the drive geometry.
   * @param maxVoltage  The maximum voltage available to the motors while
   * following the path. Should be somewhat less than the nominal battery
   * voltage (12V) to account for "voltage sag" due to current draw.
   */
  DifferentialDriveVelocitySystemConstraint(
      const LinearSystem<2, 2, 2>& system,
      const DifferentialDriveKinematics& kinematics, units::volt_t maxVoltage);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override;

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override;

 private:
  const LinearSystem<2, 2, 2>& m_system;
  const DifferentialDriveKinematics& m_kinematics;
  units::volt_t m_maxVoltage;
};
}  // namespace frc
