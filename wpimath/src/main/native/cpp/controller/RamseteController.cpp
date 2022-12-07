// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/RamseteController.h"

#include <cmath>

#include "units/angle.h"
#include "units/math.h"

using namespace frc;

/**
 * Returns sin(x) / x.
 *
 * @param x Value of which to take sinc(x).
 */
static decltype(1 / 1_rad) Sinc(units::radian_t x) {
  if (units::math::abs(x) < 1e-9_rad) {
    return decltype(1 / 1_rad){1.0 - 1.0 / 6.0 * x.value() * x.value()};
  } else {
    return units::math::sin(x) / x;
  }
}

RamseteController::RamseteController(units::unit_t<b_unit> b,
                                     units::unit_t<zeta_unit> zeta)
    : m_b{b}, m_zeta{zeta} {}

RamseteController::RamseteController()
    : RamseteController{units::unit_t<b_unit>{2.0},
                        units::unit_t<zeta_unit>{0.7}} {}

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
  const auto& eX = m_poseError.X();
  const auto& eY = m_poseError.Y();
  const auto& eTheta = m_poseError.Rotation().Radians();
  const auto& vRef = linearVelocityRef;
  const auto& omegaRef = angularVelocityRef;

  // k = 2ζ√(ω_ref² + b v_ref²)
  auto k = 2.0 * m_zeta *
           units::math::sqrt(units::math::pow<2>(omegaRef) +
                             m_b * units::math::pow<2>(vRef));

  // v_cmd = v_ref cos(e_θ) + k e_x
  // ω_cmd = ω_ref + k e_θ + b v_ref sinc(e_θ) e_y
  return ChassisSpeeds{vRef * m_poseError.Rotation().Cos() + k * eX, 0_mps,
                       omegaRef + k * eTheta + m_b * vRef * Sinc(eTheta) * eY};
}

ChassisSpeeds RamseteController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredState.velocity * desiredState.curvature);
}

void RamseteController::SetEnabled(bool enabled) {
  m_enabled = enabled;
}
