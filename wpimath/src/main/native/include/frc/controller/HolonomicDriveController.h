// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include <wpi/SymbolExports.h>

#include "frc/controller/PIDController.h"
#include "frc/controller/ProfiledPIDController.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"
#include "units/angle.h"
#include "units/angular_velocity.h"
#include "units/velocity.h"

namespace frc {
/**
 * This holonomic drive controller can be used to follow trajectories using a
 * holonomic drivetrain (i.e. swerve or mecanum). Holonomic trajectory following
 * is a much simpler problem to solve compared to skid-steer style drivetrains
 * because it is possible to individually control forward, sideways, and angular
 * velocity.
 *
 * The holonomic drive controller takes in one PID controller for each
 * direction, forward and sideways, and one profiled PID controller for the
 * angular direction. Because the heading dynamics are decoupled from
 * translations, users can specify a custom heading that the drivetrain should
 * point toward. This heading reference is profiled for smoothness.
 */
class WPILIB_DLLEXPORT HolonomicDriveController {
 public:
  /**
   * Constructs a holonomic drive controller.
   *
   * @param xController     A PID Controller to respond to error in the
   * field-relative x direction.
   * @param yController     A PID Controller to respond to error in the
   * field-relative y direction.
   * @param thetaController A profiled PID controller to respond to error in
   * angle.
   */
  constexpr HolonomicDriveController(
      PIDController xController, PIDController yController,
      ProfiledPIDController<units::radian> thetaController)
      : m_xController(std::move(xController)),
        m_yController(std::move(yController)),
        m_thetaController(std::move(thetaController)) {
    m_thetaController.EnableContinuousInput(0_deg, 360.0_deg);
  }

  constexpr HolonomicDriveController(const HolonomicDriveController&) = default;
  constexpr HolonomicDriveController& operator=(
      const HolonomicDriveController&) = default;
  constexpr HolonomicDriveController(HolonomicDriveController&&) = default;
  constexpr HolonomicDriveController& operator=(HolonomicDriveController&&) =
      default;

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  constexpr bool AtReference() const {
    const auto& eTranslate = m_poseError.Translation();
    const auto& eRotate = m_rotationError;
    const auto& tolTranslate = m_poseTolerance.Translation();
    const auto& tolRotate = m_poseTolerance.Rotation();
    return units::math::abs(eTranslate.X()) < tolTranslate.X() &&
           units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
           units::math::abs(eRotate.Radians()) < tolRotate.Radians();
  }

  /**
   * Sets the pose error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance Pose error which is tolerable.
   */
  constexpr void SetTolerance(const Pose2d& tolerance) {
    m_poseTolerance = tolerance;
  }

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose The current pose, as measured by odometry or pose
   * estimator.
   * @param trajectoryPose The desired trajectory pose, as sampled for the
   * current timestep.
   * @param desiredLinearVelocity The desired linear velocity.
   * @param desiredHeading The desired heading.
   * @return The next output of the holonomic drive controller.
   */
  constexpr ChassisSpeeds Calculate(
      const Pose2d& currentPose, const Pose2d& trajectoryPose,
      units::meters_per_second_t desiredLinearVelocity,
      const Rotation2d& desiredHeading) {
    // If this is the first run, then we need to reset the theta controller to
    // the current pose's heading.
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

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * @param currentPose The current pose, as measured by odometry or pose
   * estimator.
   * @param desiredState The desired trajectory pose, as sampled for the current
   * timestep.
   * @param desiredHeading The desired heading.
   * @return The next output of the holonomic drive controller.
   */
  constexpr ChassisSpeeds Calculate(const Pose2d& currentPose,
                                    const Trajectory::State& desiredState,
                                    const Rotation2d& desiredHeading) {
    return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                     desiredHeading);
  }

  /**
   * Enables and disables the controller for troubleshooting purposes. When
   * Calculate() is called on a disabled controller, only feedforward values
   * are returned.
   *
   * @param enabled If the controller is enabled or not.
   */
  constexpr void SetEnabled(bool enabled) { m_enabled = enabled; }

  /**
   * Returns the X PIDController
   *
   * @deprecated Use GetXController() instead.
   */
  [[deprecated("Use GetXController() instead")]]
  constexpr PIDController& getXController() {
    return m_xController;
  }

  /**
   * Returns the Y PIDController
   *
   * @deprecated Use GetYController() instead.
   */
  [[deprecated("Use GetYController() instead")]]
  constexpr PIDController& getYController() {
    return m_yController;
  }

  /**
   * Returns the rotation ProfiledPIDController
   *
   * @deprecated Use GetThetaController() instead.
   */
  [[deprecated("Use GetThetaController() instead")]]
  constexpr ProfiledPIDController<units::radian>& getThetaController() {
    return m_thetaController;
  }

  /**
   * Returns the X PIDController
   */
  constexpr PIDController& GetXController() { return m_xController; }

  /**
   * Returns the Y PIDController
   */
  constexpr PIDController& GetYController() { return m_yController; }

  /**
   * Returns the rotation ProfiledPIDController
   */
  constexpr ProfiledPIDController<units::radian>& GetThetaController() {
    return m_thetaController;
  }

 private:
  Pose2d m_poseError;
  Rotation2d m_rotationError;
  Pose2d m_poseTolerance;
  bool m_enabled = true;

  PIDController m_xController;
  PIDController m_yController;
  ProfiledPIDController<units::radian> m_thetaController;

  bool m_firstRun = true;
};
}  // namespace frc
