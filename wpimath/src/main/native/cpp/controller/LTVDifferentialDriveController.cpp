// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVDifferentialDriveController.hpp"

#include <cmath>

#include <Eigen/Core>

#include "wpi/math/controller/DifferentialDriveWheelVoltages.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/linalg/DARE.hpp"
#include "wpi/math/system/Discretization.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

using namespace wpi::math;

DifferentialDriveWheelVoltages LTVDifferentialDriveController::Calculate(
    const Pose2d& currentPose, wpi::units::meters_per_second_t leftVelocity,
    wpi::units::meters_per_second_t rightVelocity, const Pose2d& poseRef,
    wpi::units::meters_per_second_t leftVelocityRef,
    wpi::units::meters_per_second_t rightVelocityRef) {
  // This implements the linear time-varying differential drive controller in
  // theorem 8.7.4 of https://controls-in-frc.link/
  //
  //     [x ]
  //     [y ]       [Vₗ]
  // x = [θ ]   u = [Vᵣ]
  //     [vₗ]
  //     [vᵣ]

  wpi::units::meters_per_second_t velocity{(leftVelocity + rightVelocity) /
                                           2.0};

  Eigen::Vector<double, 5> r{poseRef.X().value(), poseRef.Y().value(),
                             poseRef.Rotation().Radians().value(),
                             leftVelocityRef.value(), rightVelocityRef.value()};
  Eigen::Vector<double, 5> x{currentPose.X().value(), currentPose.Y().value(),
                             currentPose.Rotation().Radians().value(),
                             leftVelocity.value(), rightVelocity.value()};

  m_error = r - x;
  m_error(2) =
      wpi::math::AngleModulus(wpi::units::radian_t{m_error(2)}).value();

  constexpr double τ = 20.0;
  Eigen::Matrix<double, 5, 5> A{
      {0.0, 0.0, 0.0, 0.5, 0.5},
      {0.0, -1 / τ, velocity.value(), 0.0, 0.0},
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

  return DifferentialDriveWheelVoltages{wpi::units::volt_t{u(0)},
                                        wpi::units::volt_t{u(1)}};
}
