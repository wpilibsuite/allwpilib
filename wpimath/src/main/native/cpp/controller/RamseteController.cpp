// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/RamseteController.h"

#include <cmath>

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

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

RamseteController::RamseteController(double b, double zeta)
    : RamseteController(units::unit_t<b_unit>{b},
                        units::unit_t<zeta_unit>{zeta}) {}

RamseteController::RamseteController(units::unit_t<b_unit> b,
                                     units::unit_t<zeta_unit> zeta)
    : m_b{b}, m_zeta{zeta} {}

RamseteController::RamseteController()
    : RamseteController(units::unit_t<b_unit>{2.0},
                        units::unit_t<zeta_unit>{0.7}) {}

void RamseteController::SetB(units::unit_t<b_unit> b) {
  m_b = b;
}

void RamseteController::SetZeta(units::unit_t<zeta_unit> zeta) {
  m_zeta = zeta;
}

units::unit_t<RamseteController::b_unit> RamseteController::GetB() const {
  return m_b;
}

units::unit_t<RamseteController::zeta_unit> RamseteController::GetZeta() const {
  return m_zeta;
}

bool RamseteController::AtReference() const {
  const auto& eTranslate = m_error.Translation();
  const auto& eRotate = m_error.Rotation();
  const auto& tolTranslate = m_tolerance.Translation();
  const auto& tolRotate = m_tolerance.Rotation();
  return units::math::abs(eTranslate.X()) < tolTranslate.X() &&
         units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
         units::math::abs(eRotate.Radians()) < tolRotate.Radians();
}

void RamseteController::SetTolerance(const Pose2d& tolerance) {
  m_tolerance = tolerance;
}

Pose2d RamseteController::GetTolerance() const {
  return m_tolerance;
}

Pose2d RamseteController::GetError() const {
  return m_error;
}

Pose2d RamseteController::GetMeasurement() const {
  return m_measurement;
}

ChassisSpeeds RamseteController::Calculate(
    const Pose2d& currentPose, const Pose2d& referencePose,
    units::meters_per_second_t linearVelocityRef,
    units::radians_per_second_t angularVelocityRef) {
  m_measurement = currentPose;
  m_reference = referencePose;

  if (!m_enabled) {
    return ChassisSpeeds{linearVelocityRef, 0_mps, angularVelocityRef};
  }

  m_error = referencePose.RelativeTo(currentPose);

  // Aliases for equation readability
  const auto& eX = m_error.X();
  const auto& eY = m_error.Y();
  const auto& eTheta = m_error.Rotation().Radians();
  const auto& vRef = linearVelocityRef;
  const auto& omegaRef = angularVelocityRef;

  auto k = 2.0 * m_zeta *
           units::math::sqrt(units::math::pow<2>(omegaRef) +
                             m_b * units::math::pow<2>(vRef));

  units::meters_per_second_t v{vRef * m_error.Rotation().Cos() + k * eX};
  units::radians_per_second_t omega{omegaRef + k * eTheta +
                                    m_b * vRef * Sinc(eTheta) * eY};
  return ChassisSpeeds{v, 0_mps, omega};
}

ChassisSpeeds RamseteController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredState.velocity * desiredState.curvature);
}

void RamseteController::SetEnabled(bool enabled) {
  m_enabled = enabled;
}

void RamseteController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("RamseteController");
  builder.AddDoubleProperty(
      "b", [this] { return GetB().to<double>(); },
      [this](double b) { SetB(units::unit_t<b_unit>{b}); });
  builder.AddDoubleProperty(
      "zeta", [this] { return GetZeta().to<double>(); },
      [this](double zeta) { SetZeta(units::unit_t<zeta_unit>{zeta}); });
  builder.AddDoubleProperty(
      "toleranceX", [this] { return m_tolerance.X().to<double>(); },
      [this](double xTolerance) {
        SetTolerance(Pose2d({units::meter_t{xTolerance}, m_tolerance.Y()},
                            m_tolerance.Rotation()));
      });
}
