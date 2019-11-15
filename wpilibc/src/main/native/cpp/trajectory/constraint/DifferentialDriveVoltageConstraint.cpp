/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/trajectory/constraint/DifferentialDriveVoltageConstraint.h"

#include <algorithm>
#include <limits>

#include <wpi/MathExtras.h>

using namespace frc;

DifferentialDriveVoltageConstraint::DifferentialDriveVoltageConstraint(
    SimpleMotorFeedforward<units::meter> feedforward,
    DifferentialDriveKinematics kinematics, units::volt_t maxVoltage)
    : m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_maxVoltage(maxVoltage) {}

units::meters_per_second_t DifferentialDriveVoltageConstraint::MaxVelocity(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t velocity) {
  return units::meters_per_second_t(std::numeric_limits<double>::max());
}

TrajectoryConstraint::MinMax
DifferentialDriveVoltageConstraint::MinMaxAcceleration(
    const Pose2d& pose, curvature_t curvature,
    units::meters_per_second_t speed) {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({speed, 0_mps, speed * curvature});

  auto maxWheelSpeed = std::max(wheelSpeeds.left, wheelSpeeds.right);
  auto minWheelSpeed = std::min(wheelSpeeds.left, wheelSpeeds.right);

  auto maxWheelAcceleration =
      (m_maxVoltage - m_feedforward.kS * wpi::sgn(maxWheelSpeed) -
       m_feedforward.kV * maxWheelSpeed) /
      m_feedforward.kA;
  auto minWheelAcceleration =
      (-m_maxVoltage - m_feedforward.kS * wpi::sgn(minWheelSpeed) -
       m_feedforward.kV * minWheelSpeed) /
      m_feedforward.kA;

  // If moving forward, max acceleration constraint corresponds to wheel on
  // outside of turn If moving backward, max acceleration constraint corresponds
  // to wheel on inside of turn
  auto maxChassisAcceleration =
      maxWheelAcceleration /
      (1 + m_kinematics.trackWidth * units::math::abs(curvature) *
               wpi::sgn(speed) / (2_rad));
  auto minChassisAcceleration =
      minWheelAcceleration /
      (1 - m_kinematics.trackWidth * units::math::abs(curvature) *
               wpi::sgn(speed) / (2_rad));

  // Negate min chassis acceleration if center of turn is inside of wheelbase
  if ((m_kinematics.trackWidth / 2) > 1_rad / units::math::abs(curvature)) {
    minChassisAcceleration = -minChassisAcceleration;
  }

  return {minChassisAcceleration, maxChassisAcceleration};
}
