/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/LTVUnicycleController.h"

#include <cmath>

#include <wpi/MathExtras.h>

#include "frc/controller/LinearQuadraticRegulator.h"

using namespace frc;

LTVUnicycleController::LTVUnicycleController(
    const std::array<double, 3>& Qelems, const std::array<double, 2>& Relems,
    units::second_t dt) {
  Eigen::Matrix<double, 3, 3> A0;
  A0 << 0, 0, 0, 0, 0, 1e-9, 0, 0, 0;
  Eigen::Matrix<double, 3, 3> A1;
  A1 << 0, 0, 0, 0, 0, 1, 0, 0, 0;
  Eigen::Matrix<double, 3, 2> B;
  B << 1, 0, 0, 0, 0, 1;

  m_K0 = LinearQuadraticRegulator<3, 2>(A0, B, Qelems, Relems, dt).K();
  m_K1 = LinearQuadraticRegulator<3, 2>(A1, B, Qelems, Relems, dt).K();
}

bool LTVUnicycleController::AtReference() const {
  const auto& eTranslate = m_poseError.Translation();
  const auto& eRotate = m_poseError.Rotation();
  const auto& tolTranslate = m_poseTolerance.Translation();
  const auto& tolRotate = m_poseTolerance.Rotation();
  return units::math::abs(eTranslate.X()) < tolTranslate.X() &&
         units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
         units::math::abs(eRotate.Radians()) < tolRotate.Radians();
}

void LTVUnicycleController::SetTolerance(const Pose2d& poseTolerance) {
  m_poseTolerance = poseTolerance;
}

ChassisSpeeds LTVUnicycleController::Calculate(
    const Pose2d& currentPose, const Pose2d& poseRef,
    units::meters_per_second_t linearVelocityRef,
    units::radians_per_second_t angularVelocityRef) {
  Eigen::Matrix<double, 3, 1> x;
  x(0, 0) = currentPose.Translation().X().to<double>();
  x(1, 0) = currentPose.Translation().Y().to<double>();
  x(2, 0) = currentPose.Rotation().Radians().to<double>();

  Eigen::Matrix<double, 3, 1> r;
  r(0, 0) = poseRef.Translation().X().to<double>();
  r(1, 0) = poseRef.Translation().Y().to<double>();
  r(2, 0) = poseRef.Rotation().Radians().to<double>();

  m_poseError = poseRef.RelativeTo(currentPose);

  double kx = m_K0(0, 0);
  double ky0 = m_K0(0, 1);
  double ky1 = m_K1(0, 1);
  double ktheta1 = m_K1(0, 2);

  double v = linearVelocityRef.to<double>();
  double sqrtAbsV = std::sqrt(std::abs(v));

  Eigen::Matrix<double, 2, 3> K;
  K(0, 0) = kx;
  K(0, 1) = 0;
  K(0, 2) = 0;
  K(1, 0) = 0;
  K(1, 1) = (ky0 + (ky1 - ky0) * sqrtAbsV) * wpi::sgn(v);
  K(1, 2) = ktheta1 * sqrtAbsV;

  Eigen::Matrix<double, 3, 3> inRobotFrame =
      Eigen::Matrix<double, 3, 3>::Identity();
  inRobotFrame(0, 0) = std::cos(x(2, 0));
  inRobotFrame(0, 1) = std::sin(x(2, 0));
  inRobotFrame(1, 0) = -std::sin(x(2, 0));
  inRobotFrame(1, 1) = std::cos(x(2, 0));
  auto u = K * inRobotFrame * (r - x);

  return ChassisSpeeds{
      linearVelocityRef + units::meters_per_second_t{u(0, 0)}, 0_mps,
      angularVelocityRef + units::radians_per_second_t{u(1, 0)}};
}

ChassisSpeeds LTVUnicycleController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredState.velocity * desiredState.curvature);
}
