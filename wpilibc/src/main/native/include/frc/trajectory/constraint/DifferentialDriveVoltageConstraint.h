/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <units/units.h>

#include "frc/controller/SimpleMotorFeedforward.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"

namespace frc {
/**
 * A class that enforces constraints on differential drive voltage expenditure
 * based on the motor dynamics and the drive kinematics.  Ensures that the
 * acceleration of any wheel of the robot while following the trajectory is
 * never higher than what can be achieved with the given maximum voltage.
 */
class DifferentialDriveVoltageConstraint : public TrajectoryConstraint {
 public:
  /**
   * Creates a new DifferentialDriveVoltageConstraint.
   *
   * @param feedforward A feedforward component describing the behavior of the
   * drive.
   * @param kinematics  A kinematics component describing the drive geometry.
   * @param maxVoltage  The maximum voltage available to the motors while
   * following the path. Should be somewhat less than the nominal battery
   * voltage (12V) to account for "voltage sag" due to current draw.
   */
  DifferentialDriveVoltageConstraint(
      SimpleMotorFeedforward<units::meter> feedforward,
      DifferentialDriveKinematics kinematics, units::volt_t maxVoltage);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, curvature_t curvature,
      units::meters_per_second_t velocity) override;

  MinMax MinMaxAcceleration(const Pose2d& pose, curvature_t curvature,
                            units::meters_per_second_t speed) override;

 private:
  SimpleMotorFeedforward<units::meter> m_feedforward;
  DifferentialDriveKinematics m_kinematics;
  units::volt_t m_maxVoltage;
};
}  // namespace frc
