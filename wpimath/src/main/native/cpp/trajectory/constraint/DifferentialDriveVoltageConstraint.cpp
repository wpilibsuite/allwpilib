// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/trajectory/constraint/DifferentialDriveVoltageConstraint.h"

#include <wpi/MathExtras.h>

#include "Eigen/Core"
#include "units/math.h"

using namespace frc;

DifferentialDriveVoltageConstraint::DifferentialDriveVoltageConstraint(
    const SimpleMotorFeedforward<units::meter>& feedforward,
    const DifferentialDriveKinematics& kinematics, units::volt_t maxVoltage)
    : m_feedforward(feedforward),
      m_kinematics(kinematics),
      m_maxVoltage(maxVoltage) {}

units::meters_per_second_t DifferentialDriveVoltageConstraint::MaxVelocity(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t velocity) const {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({velocity, 0_mps, velocity * curvature});
  wheelSpeeds.Normalize(velocity);

  return m_kinematics.ToChassisSpeeds(wheelSpeeds).vx;
}

TrajectoryConstraint::MinMax
DifferentialDriveVoltageConstraint::MinMaxAcceleration(
    const Pose2d& pose, units::curvature_t curvature,
    units::meters_per_second_t speed) const {
  auto wheelSpeeds =
      m_kinematics.ToWheelSpeeds({speed, 0_mps, speed * curvature});

  // See section 15.1 of https://tavsys.net/controls-in-frc
  const double& Kv = m_feedforward.kV();
  const double& Ka = m_feedforward.kA();
  Eigen::DiagonalMatrix<double, 2> A;
  A.diagonal() << -Kv / Ka, -Kv / Ka;
  Eigen::DiagonalMatrix<double, 2> B;
  B.diagonal() << 1.0 / Ka, 1.0 / Ka;

  Eigen::Vector2d x;
  x << wheelSpeeds.left.to<double>(), wheelSpeeds.right.to<double>();

  // Get dx/dt for min u
  Eigen::Vector2d u;
  u << -m_maxVoltage.to<double>(), -m_maxVoltage.to<double>();
  auto minAccel = GetAcceleration(A, B, x, u);

  // Get dx/dt for max u
  u << m_maxVoltage.to<double>(), m_maxVoltage.to<double>();
  auto maxAccel = GetAcceleration(A, B, x, u);

  return {minAccel, maxAccel};
}

units::meters_per_second_squared_t
DifferentialDriveVoltageConstraint::GetAcceleration(
    const Eigen::Matrix2d& A, const Eigen::Matrix2d& B,
    const Eigen::Vector2d& x, const Eigen::Vector2d& u) const {
  const double& Ks = m_feedforward.kS();
  const double& Ka = m_feedforward.kA();

  Eigen::Vector2d xDot = A * x + B * u;
  xDot(0, 0) -= Ks / Ka * wpi::sgn(x(0, 0));
  xDot(1, 0) -= Ks / Ka * wpi::sgn(x(1, 0));
  return units::meters_per_second_squared_t{(xDot(0, 0) + xDot(1, 0)) / 2.0};
}
