// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/LTVUnicycleController.h"

#include "frc/StateSpaceUtil.h"
#include "frc/controller/LinearQuadraticRegulator.h"
#include "units/math.h"

using namespace frc;

/**
 * States of the drivetrain system.
 */
class State {
 public:
  /// X position in global coordinate frame.
  static constexpr int kX = 0;

  /// Y position in global coordinate frame.
  static constexpr int kY = 1;

  /// Heading in global coordinate frame.
  static constexpr int kHeading = 2;
};

/**
 * Inputs of the drivetrain system.
 */
class Input {
 public:
  /// Linear velocity.
  static constexpr int kLinearVelocity = 3;

  /// Angular velocity.
  static constexpr int kAngularVelocity = 4;
};

LTVUnicycleController::LTVUnicycleController(
    const wpi::array<double, 3>& Qelems, const wpi::array<double, 2>& Relems,
    units::second_t dt)
    : m_dt{dt} {
  m_Q = frc::MakeCostMatrix(Qelems);
  m_R = frc::MakeCostMatrix(Relems);
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
  if (!m_enabled) {
    return ChassisSpeeds{linearVelocityRef, 0_mps, angularVelocityRef};
  }

  m_poseError = poseRef.RelativeTo(currentPose);

  if (units::math::abs(linearVelocityRef) < 1e-4_mps) {
    m_A(State::kY, State::kHeading) = 1e-4;
  } else {
    m_A(State::kY, State::kHeading) = linearVelocityRef.value();
  }
  Vectord<3> e{m_poseError.X().value(), m_poseError.Y().value(),
               m_poseError.Rotation().Radians().value()};
  Vectord<2> u =
      frc::LinearQuadraticRegulator<3, 2>{m_A, m_B, m_Q, m_R, m_dt}.K() * e;

  return ChassisSpeeds{linearVelocityRef + units::meters_per_second_t{u(0)},
                       0_mps,
                       angularVelocityRef + units::radians_per_second_t{u(1)}};
}

ChassisSpeeds LTVUnicycleController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredState.velocity * desiredState.curvature);
}

void LTVUnicycleController::SetEnabled(bool enabled) {
  m_enabled = enabled;
}
