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

  // Calculate maximum/minimum possible accelerations from motor dynamics
  // and max/min wheel speeds
  auto maxWheelAcceleration =
      m_feedforward.MaxAchievableAcceleration(m_maxVoltage, maxWheelSpeed);
  auto minWheelAcceleration =
      m_feedforward.MinAchievableAcceleration(m_maxVoltage, minWheelSpeed);

  // Robot chassis turning on radius = 1/|curvature|.  Outer wheel has radius
  // increased by half of the trackwidth T.  Inner wheel has radius decreased
  // by half of the trackwidth.  Achassis / radius = Aouter / (radius + T/2), so
  // Achassis = Aouter * radius / (radius + T/2) = Aouter / (1 + |curvature|T/2)
  // Inner wheel is similar.

  // sgn(speed) term added to correctly account for which wheel is on
  // outside of turn:
  // If moving forward, max acceleration constraint corresponds to wheel on
  // outside of turn
  // If moving backward, max acceleration constraint corresponds to wheel on
  // inside of turn
  auto maxChassisAcceleration =
      maxWheelAcceleration /
      (1 + m_kinematics.trackWidth * units::math::abs(curvature) *
               wpi::sgn(speed) / (2_rad));
  auto minChassisAcceleration =
      minWheelAcceleration /
      (1 - m_kinematics.trackWidth * units::math::abs(curvature) *
               wpi::sgn(speed) / (2_rad));

  // Negate acceleration corresponding to wheel on inside of turn
  // if center of turn is inside of wheelbase
  if ((m_kinematics.trackWidth / 2) > 1_rad / units::math::abs(curvature)) {
    if (speed > 0_mps) {
      minChassisAcceleration = -minChassisAcceleration;
    } else {
      maxChassisAcceleration = -maxChassisAcceleration;
    }
  }

  return {minChassisAcceleration, maxChassisAcceleration};
}
