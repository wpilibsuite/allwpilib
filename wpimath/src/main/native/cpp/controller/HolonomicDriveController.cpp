// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/controller/HolonomicDriveController.h"

#include <utility>

#include <wpi/sendable/SendableBuilder.h>
#include <wpi/sendable/SendableRegistry.h>

#include "units/angular_velocity.h"

using namespace frc;

HolonomicDriveController::HolonomicDriveController(
    frc2::PIDController xController, frc2::PIDController yController,
    ProfiledPIDController<units::radian> thetaController)
    : m_xController(std::move(xController)),
      m_yController(std::move(yController)),
      m_thetaController(std::move(thetaController)) {
  wpi::SendableRegistry::AddChild(this, &xController);
  wpi::SendableRegistry::AddChild(this, &yController);
  wpi::SendableRegistry::AddChild(this, &thetaController);
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
    const Pose2d& currentPose, const Pose2d& poseRef,
    units::meters_per_second_t linearVelocityRef, const Rotation2d& angleRef) {
  // If this is the first run, then we need to reset the theta controller to the
  // current pose's heading.
  if (m_firstRun) {
    m_thetaController.Reset(currentPose.Rotation().Radians());
    m_firstRun = false;
  }

  // Calculate feedforward velocities (field-relative)
  auto xFF = linearVelocityRef * poseRef.Rotation().Cos();
  auto yFF = linearVelocityRef * poseRef.Rotation().Sin();
  auto thetaFF = units::radians_per_second_t(m_thetaController.Calculate(
      currentPose.Rotation().Radians(), angleRef.Radians()));

  m_poseError = poseRef.RelativeTo(currentPose);
  m_rotationError = angleRef - currentPose.Rotation();

  if (!m_enabled) {
    return ChassisSpeeds::FromFieldRelativeSpeeds(xFF, yFF, thetaFF,
                                                  currentPose.Rotation());
  }

  // Calculate feedback velocities (based on position error).
  auto xFeedback = units::meters_per_second_t(
      m_xController.Calculate(currentPose.X().value(), poseRef.X().value()));
  auto yFeedback = units::meters_per_second_t(
      m_yController.Calculate(currentPose.Y().value(), poseRef.Y().value()));

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

void HolonomicDriveController::SetEnabled(bool enabled) {
  m_enabled = enabled;
}

void HolonomicDriveController::InitSendable(wpi::SendableBuilder& builder) {
  builder.SetSmartDashboardType("HolonomicDriveController");
  builder.AddDoubleProperty(
      "toleranceX", [this] { return m_poseTolerance.X().to<double>(); },
      [this](double xTolerance) {
        SetTolerance(Pose2d({units::meter_t{xTolerance}, m_poseTolerance.Y()},
                            m_poseTolerance.Rotation()));
      });
  builder.AddDoubleProperty(
      "toleranceY", [this] { return m_poseTolerance.Y().to<double>(); },
      [this](double yTolerance) {
        SetTolerance(Pose2d({m_poseTolerance.X(), units::meter_t{yTolerance}},
                            m_poseTolerance.Rotation()));
      });
  builder.AddDoubleProperty(
      "toleranceDegrees",
      [this] { return m_poseTolerance.Rotation().Degrees().to<double>(); },
      [this](double degreesTolerance) {
        SetTolerance(Pose2d(m_poseTolerance.Translation(),
                            {units::degree_t(degreesTolerance)}));
      });
  builder.AddDoubleProperty(
      "referenceX", [this] { return m_poseReference.X().to<double>(); },
      nullptr);
  builder.AddDoubleProperty(
      "referenceY", [this] { return m_poseReference.Y().to<double>(); },
      nullptr);
  builder.AddDoubleProperty(
      "referenceDegrees",
      [this] { return m_headingReference.Degrees().to<double>(); }, nullptr);
  builder.AddDoubleProperty(
      "measurementX", [this] { return m_measurement.X().to<double>(); },
      nullptr);
  builder.AddDoubleProperty(
      "measurementY", [this] { return m_measurement.Y().to<double>(); },
      nullptr);
  builder.AddDoubleProperty(
      "measurementDegrees",
      [this] { return m_measurement.Rotation().Degrees().to<double>(); },
      nullptr);
  builder.AddDoubleProperty(
      "errorX", [this] { return m_poseError.X().to<double>(); }, nullptr);
  builder.AddDoubleProperty(
      "errorY", [this] { return m_poseError.Y().to<double>(); }, nullptr);
  builder.AddDoubleProperty(
      "errorDegrees",
      [this] { return m_poseError.Rotation().Degrees().to<double>(); },
      nullptr);
  builder.AddBooleanProperty(
      "atReference", [this] { return AtReference(); }, nullptr);
}
