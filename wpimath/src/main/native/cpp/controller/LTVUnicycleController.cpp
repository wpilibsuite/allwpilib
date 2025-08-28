// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVUnicycleController.h"

#include "units/math.h"
#include "wpi/math/DARE.h"
#include "wpi/math/system/Discretization.h"

using namespace wpi::math;

ChassisSpeeds LTVUnicycleController::Calculate(
    const Pose2d& currentPose, const Pose2d& poseRef,
    units::meters_per_second_t linearVelocityRef,
    units::radians_per_second_t angularVelocityRef) {
  // The change in global pose for a unicycle is defined by the following three
  // equations.
  //
  // ẋ = v cosθ
  // ẏ = v sinθ
  // θ̇ = ω
  //
  // Here's the model as a vector function where x = [x  y  θ]ᵀ and u = [v  ω]ᵀ.
  //
  //           [v cosθ]
  // f(x, u) = [v sinθ]
  //           [  ω   ]
  //
  // To create an LQR, we need to linearize this.
  //
  //               [0  0  −v sinθ]                  [cosθ  0]
  // ∂f(x, u)/∂x = [0  0   v cosθ]    ∂f(x, u)/∂u = [sinθ  0]
  //               [0  0     0   ]                  [ 0    1]
  //
  // We're going to make a cross-track error controller, so we'll apply a
  // clockwise rotation matrix to the global tracking error to transform it into
  // the robot's coordinate frame. Since the cross-track error is always
  // measured from the robot's coordinate frame, the model used to compute the
  // LQR should be linearized around θ = 0 at all times.
  //
  //     [0  0  −v sin0]        [cos0  0]
  // A = [0  0   v cos0]    B = [sin0  0]
  //     [0  0     0   ]        [ 0    1]
  //
  //     [0  0  0]              [1  0]
  // A = [0  0  v]          B = [0  0]
  //     [0  0  0]              [0  1]

  if (!m_enabled) {
    return ChassisSpeeds{linearVelocityRef, 0_mps, angularVelocityRef};
  }

  // The DARE is ill-conditioned if the velocity is close to zero, so don't
  // let the system stop.
  if (units::math::abs(linearVelocityRef) < 1e-4_mps) {
    linearVelocityRef = 1e-4_mps;
  }

  m_poseError = poseRef.RelativeTo(currentPose);

  Eigen::Matrix<double, 3, 3> A{
      {0.0, 0.0, 0.0}, {0.0, 0.0, linearVelocityRef.value()}, {0.0, 0.0, 0.0}};
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

  return ChassisSpeeds{linearVelocityRef + units::meters_per_second_t{u(0)},
                       0_mps,
                       angularVelocityRef + units::radians_per_second_t{u(1)}};
}
