/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/RamseteController.h"

#include <cmath>

#include <units/math.h>

using namespace frc;

/**
 * Returns sin(x) / x.
 *
 * @param x Value of which to take sinc(x).
 */
static double Sinc(double x) {
  if (std::abs(x) < 1e-9) {
    return 1.0 - 1.0 / 6.0 * x * x;
  } else {
    return std::sin(x) / x;
  }
}

RamseteController::RamseteController(double b, double zeta)
    : m_b{b}, m_zeta{zeta} {}

bool RamseteController::AtReference() const {
  const auto& eTranslate = m_poseError.Translation();
  const auto& eRotate = m_poseError.Rotation();
  const auto& tolTranslate = m_poseTolerance.Translation();
  const auto& tolRotate = m_poseTolerance.Rotation();
  return units::math::abs(eTranslate.X()) < tolTranslate.X() &&
         units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
         units::math::abs(eRotate.Radians()) < tolRotate.Radians();
}

void RamseteController::SetTolerance(const Pose2d& poseTolerance) {
  m_poseTolerance = poseTolerance;
}

ChassisSpeeds RamseteController::Calculate(
    const Pose2d& currentPose, const Pose2d& poseRef,
    units::meters_per_second_t linearVelocityRef,
    units::radians_per_second_t angularVelocityRef) {
  if (!m_enabled) {
    return ChassisSpeeds{linearVelocityRef, 0_mps, angularVelocityRef};
  }

  m_poseError = poseRef.RelativeTo(currentPose);

  // Aliases for equation readability
  double eX = m_poseError.X().to<double>();
  double eY = m_poseError.Y().to<double>();
  double eTheta = m_poseError.Rotation().Radians().to<double>();
  double vRef = linearVelocityRef.to<double>();
  double omegaRef = angularVelocityRef.to<double>();

  double k =
      2.0 * m_zeta * std::sqrt(std::pow(omegaRef, 2) + m_b * std::pow(vRef, 2));

  units::meters_per_second_t v{vRef * m_poseError.Rotation().Cos() + k * eX};
  units::radians_per_second_t omega{omegaRef + k * eTheta +
                                    m_b * vRef * Sinc(eTheta) * eY};
  return ChassisSpeeds{v, 0_mps, omega};
}

ChassisSpeeds RamseteController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredState.velocity * desiredState.curvature);
}

void RamseteController::SetEnabled(bool enabled) { m_enabled = enabled; }
