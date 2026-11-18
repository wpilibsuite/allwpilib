// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVUnicycleController.hpp"

#include <Eigen/Core>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/linalg/DARE.hpp"
#include "wpi/math/system/Discretization.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

ChassisVelocities LTVUnicycleController::Calculate(
    const Pose2d& currentPose, const Pose2d& poseRef,
    wpi::units::meters_per_second_t linearVelocityRef,
    wpi::units::radians_per_second_t angularVelocityRef) {
  // See theorem 8.8.1 of
  // https://file.tavsys.net/control/controls-engineering-in-frc.pdf

  if (!m_enabled) {
    return ChassisVelocities{linearVelocityRef, 0_mps, angularVelocityRef};
  }

  m_poseError = poseRef.RelativeTo(currentPose);

  constexpr double τ = 20.0;
  Eigen::Matrix<double, 3, 3> A{{0.0, 0.0, 0.0},
                                {0.0, -1 / τ, linearVelocityRef.value()},
                                {0.0, 0.0, 0.0}};
  constexpr Eigen::Matrix<double, 3, 2> B{{1.0, 0.0}, {0.0, 0.0}, {0.0, 1.0}};

  Eigen::Matrix<double, 3, 3> discA;
  Eigen::Matrix<double, 3, 2> discB;
  DiscretizeAB(A, B, m_dt, &discA, &discB);

  auto S = DARE<3, 2>(discA, discB, m_Q, m_R, false).value();

  // K = (BᵀSB + R)⁻¹BᵀSA
  Eigen::Matrix<double, 2, 3> K = (discB.transpose() * S * discB + m_R)
                                      .llt()
                                      .solve(discB.transpose() * S * discA);

  Eigen::Vector3d e{m_poseError.X().value(), m_poseError.Y().value(),
                    m_poseError.Rotation().Radians().value()};
  Eigen::Vector2d u = K * e;

  return ChassisVelocities{
      linearVelocityRef + wpi::units::meters_per_second_t{u(0)}, 0_mps,
      angularVelocityRef + wpi::units::radians_per_second_t{u(1)}};
}
