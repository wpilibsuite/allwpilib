// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVDifferentialDriveController.hpp"

#include <cmath>

#include "wpi/math/linalg/DARE.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/math/system/Discretization.hpp"

using namespace frc;

DifferentialDriveWheelVoltages LTVDifferentialDriveController::Calculate(
    const Pose2d& currentPose, units::meters_per_second_t leftVelocity,
    units::meters_per_second_t rightVelocity, const Pose2d& poseRef,
    units::meters_per_second_t leftVelocityRef,
    units::meters_per_second_t rightVelocityRef) {
  // This implements the linear time-varying differential drive controller in
  // theorem 8.7.4 of https://controls-in-frc.link/
  //
  //     [x ]
  //     [y ]       [Vₗ]
  // x = [θ ]   u = [Vᵣ]
  //     [vₗ]
  //     [vᵣ]

  units::meters_per_second_t velocity{(leftVelocity + rightVelocity) / 2.0};

  // The DARE is ill-conditioned if the velocity is close to zero, so don't
  // let the system stop.
  if (units::math::abs(velocity) < 1e-4_mps) {
    velocity = 1e-4_mps;
  }

  Eigen::Vector<double, 5> r{poseRef.X().value(), poseRef.Y().value(),
                             poseRef.Rotation().Radians().value(),
                             leftVelocityRef.value(), rightVelocityRef.value()};
  Eigen::Vector<double, 5> x{currentPose.X().value(), currentPose.Y().value(),
                             currentPose.Rotation().Radians().value(),
                             leftVelocity.value(), rightVelocity.value()};

  m_error = r - x;
  m_error(2) = frc::AngleModulus(units::radian_t{m_error(2)}).value();

  Eigen::Matrix<double, 5, 5> A{
      {0.0, 0.0, 0.0, 0.5, 0.5},
      {0.0, 0.0, velocity.value(), 0.0, 0.0},
      {0.0, 0.0, 0.0, -1.0 / m_trackwidth.value(), 1.0 / m_trackwidth.value()},
      {0.0, 0.0, 0.0, m_A(0, 0), m_A(0, 1)},
      {0.0, 0.0, 0.0, m_A(1, 0), m_A(1, 1)}};
  Eigen::Matrix<double, 5, 2> B{{0.0, 0.0},
                                {0.0, 0.0},
                                {0.0, 0.0},
                                {m_B(0, 0), m_B(0, 1)},
                                {m_B(1, 0), m_B(1, 1)}};

  Eigen::Matrix<double, 5, 5> discA;
  Eigen::Matrix<double, 5, 2> discB;
  DiscretizeAB(A, B, m_dt, &discA, &discB);

  auto S = DARE<5, 2>(discA, discB, m_Q, m_R, false).value();

  // K = (BᵀSB + R)⁻¹BᵀSA
  Eigen::Matrix<double, 2, 5> K = (discB.transpose() * S * discB + m_R)
                                      .llt()
                                      .solve(discB.transpose() * S * discA);

  Eigen::Matrix<double, 5, 5> inRobotFrame{
      {std::cos(x(2)), std::sin(x(2)), 0.0, 0.0, 0.0},
      {-std::sin(x(2)), std::cos(x(2)), 0.0, 0.0, 0.0},
      {0.0, 0.0, 1.0, 0.0, 0.0},
      {0.0, 0.0, 0.0, 1.0, 0.0},
      {0.0, 0.0, 0.0, 0.0, 1.0}};

  Eigen::Vector2d u = K * inRobotFrame * m_error;

  return DifferentialDriveWheelVoltages{units::volt_t{u(0)},
                                        units::volt_t{u(1)}};
}
