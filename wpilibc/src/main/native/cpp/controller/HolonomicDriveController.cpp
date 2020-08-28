/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/HolonomicDriveController.h"

#include <units/angular_velocity.h>

using namespace frc;

HolonomicDriveController::HolonomicDriveController(
    const frc2::PIDController& xController,
    const frc2::PIDController& yController,
    const ProfiledPIDController<units::radian>& thetaController)
    : m_xController(xController),
      m_yController(yController),
      m_thetaController(thetaController) {}

bool HolonomicDriveController::AtReference() const {
  const auto& eTranslate = m_poseError.Translation();
  const auto& eRotate = m_poseError.Rotation();
  const auto& tolTranslate = m_poseTolerance.Translation();
  const auto& tolRotate = m_poseTolerance.Rotation();
  return units::math::abs(eTranslate.X()) < tolTranslate.X() &&
         units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
         units::math::abs(eRotate.Radians()) < tolRotate.Radians();
}

void HolonomicDriveController::SetTolerance(const Pose2d& tolerance) {
  m_poseTolerance = tolerance;
}

ChassisSpeeds HolonomicDriveController::Calculate(
    const Pose2d& currentPose, const Pose2d& poseRef,
    units::meters_per_second_t linearVelocityRef, const Rotation2d& angleRef) {
  // Calculate feedforward velocities (field-relative)
  auto xFF = linearVelocityRef * poseRef.Rotation().Cos();
  auto yFF = linearVelocityRef * poseRef.Rotation().Sin();
  auto thetaFF = units::radians_per_second_t(m_thetaController.Calculate(
      currentPose.Rotation().Radians(), angleRef.Radians()));

  m_poseError = poseRef.RelativeTo(currentPose);

  if (!m_enabled) {
    return ChassisSpeeds::FromFieldRelativeSpeeds(xFF, yFF, thetaFF,
                                                  currentPose.Rotation());
  }

  // Calculate feedback velocities (based on position error).
  auto xFeedback = units::meters_per_second_t(m_xController.Calculate(
      currentPose.X().to<double>(), poseRef.X().to<double>()));
  auto yFeedback = units::meters_per_second_t(m_yController.Calculate(
      currentPose.Y().to<double>(), poseRef.Y().to<double>()));

  // Return next output.
  return ChassisSpeeds::FromFieldRelativeSpeeds(
      xFF + xFeedback, yFF + yFeedback, thetaFF, currentPose.Rotation());
}

ChassisSpeeds HolonomicDriveController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState,
    const Rotation2d& angleRef) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   angleRef);
}

void HolonomicDriveController::SetEnabled(bool enabled) { m_enabled = enabled; }
