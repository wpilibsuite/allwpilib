// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/controller/SimpleMotorFeedforward.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "units/length.h"
#include "units/voltage.h"

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
      const SimpleMotorFeedforward<units::meter>& feedforward,
      const DifferentialDriveKinematics& kinematics, units::volt_t maxVoltage);

  units::meters_per_second_t MaxVelocity(
      const Pose2d& pose, units::curvature_t curvature,
      units::meters_per_second_t velocity) const override;

  MinMax MinMaxAcceleration(const Pose2d& pose, units::curvature_t curvature,
                            units::meters_per_second_t speed) const override;

 private:
  const SimpleMotorFeedforward<units::meter>& m_feedforward;
  const DifferentialDriveKinematics& m_kinematics;
  units::volt_t m_maxVoltage;
};
}  // namespace frc
