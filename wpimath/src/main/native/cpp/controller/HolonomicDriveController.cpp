// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/HolonomicDriveController.h"

#include <utility>

#include "units/angular_velocity.h"

using namespace frc;

HolonomicDriveController::HolonomicDriveController(
    PIDController xController, PIDController yController,
    ProfiledPIDController<units::radian> thetaController)
    : m_xController(std::move(xController)),
      m_yController(std::move(yController)),
      m_thetaController(std::move(thetaController)) {
  m_thetaController.EnableContinuousInput(0_deg, 360.0_deg);
}

bool HolonomicDriveController::AtReference() const {
  const auto& eTranslate = m_poseError.Translation();
  const auto& eRotate = m_rotationError;
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
    const Pose2d& currentPose, const Pose2d& trajectoryPose,
    units::meters_per_second_t desiredLinearVelocity,
    const Rotation2d& desiredHeading) {
  // If this is the first run, then we need to reset the theta controller to the
  // current pose's heading.
  if (m_firstRun) {
    m_thetaController.Reset(currentPose.Rotation().Radians());
    m_firstRun = false;
  }

  // Calculate feedforward velocities (field-relative)
  auto xFF = desiredLinearVelocity * trajectoryPose.Rotation().Cos();
  auto yFF = desiredLinearVelocity * trajectoryPose.Rotation().Sin();
  auto thetaFF = units::radians_per_second_t{m_thetaController.Calculate(
      currentPose.Rotation().Radians(), desiredHeading.Radians())};

  m_poseError = trajectoryPose.RelativeTo(currentPose);
  m_rotationError = desiredHeading - currentPose.Rotation();

  if (!m_enabled) {
    return ChassisSpeeds::FromFieldRelativeSpeeds(xFF, yFF, thetaFF,
                                                  currentPose.Rotation());
  }

  // Calculate feedback velocities (based on position error).
  auto xFeedback = units::meters_per_second_t{m_xController.Calculate(
      currentPose.X().value(), trajectoryPose.X().value())};
  auto yFeedback = units::meters_per_second_t{m_yController.Calculate(
      currentPose.Y().value(), trajectoryPose.Y().value())};

  // Return next output.
  return ChassisSpeeds::FromFieldRelativeSpeeds(
      xFF + xFeedback, yFF + yFeedback, thetaFF, currentPose.Rotation());
}

ChassisSpeeds HolonomicDriveController::Calculate(
    const Pose2d& currentPose, const Trajectory::State& desiredState,
    const Rotation2d& desiredHeading) {
  return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                   desiredHeading);
}

void HolonomicDriveController::SetEnabled(bool enabled) {
  m_enabled = enabled;
}

PIDController& HolonomicDriveController::getXController() {
  return m_xController;
}

PIDController& HolonomicDriveController::getYController() {
  return m_yController;
}

ProfiledPIDController<units::radian>&
HolonomicDriveController::getThetaController() {
  return m_thetaController;
}

PIDController& HolonomicDriveController::GetXController() {
  return m_xController;
}

PIDController& HolonomicDriveController::GetYController() {
  return m_yController;
}

ProfiledPIDController<units::radian>&
HolonomicDriveController::GetThetaController() {
  return m_thetaController;
}
