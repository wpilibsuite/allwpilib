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
    units::second_t dt, units::meters_per_second_t maxVelocity)
    : LTVUnicycleController{{0.0625, 0.125, 2.0}, {1.0, 2.0}, dt, maxVelocity} {
}

LTVUnicycleController::LTVUnicycleController(
    const wpi::array<double, 3>& Qelems, const wpi::array<double, 2>& Relems,
    units::second_t dt, units::meters_per_second_t maxVelocity) {
  Matrixd<3, 3> A = Matrixd<3, 3>::Zero();
  Matrixd<3, 2> B{{1.0, 0.0}, {0.0, 0.0}, {0.0, 1.0}};
  Matrixd<3, 3> Q = frc::MakeCostMatrix(Qelems);
  Matrixd<2, 2> R = frc::MakeCostMatrix(Relems);

  for (auto velocity = -maxVelocity; velocity < maxVelocity;
       velocity += 0.01_mps) {
    // The DARE is ill-conditioned if the velocity is close to zero, so don't
    // let the system stop.
    if (units::math::abs(velocity) < 1e-4_mps) {
      m_table.insert(velocity, Matrixd<2, 3>::Zero());
    } else {
      A(State::kY, State::kHeading) = velocity.value();
      m_table.insert(velocity,
                     frc::LinearQuadraticRegulator<3, 2>{A, B, Q, R, dt}.K());
    }
  }
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

  const auto& K = m_table[linearVelocityRef];
  Vectord<3> e{m_poseError.X().value(), m_poseError.Y().value(),
               m_poseError.Rotation().Radians().value()};
  Vectord<2> u = K * e;

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
