// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/controller/PIDController.h"
#include "frc/controller/ProfiledPIDController.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"
#include "units/angle.h"
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
  HolonomicDriveController(
      PIDController xController, PIDController yController,
      ProfiledPIDController<units::radian> thetaController);

  HolonomicDriveController(const HolonomicDriveController&) = default;
  HolonomicDriveController& operator=(const HolonomicDriveController&) =
      default;
  HolonomicDriveController(HolonomicDriveController&&) = default;
  HolonomicDriveController& operator=(HolonomicDriveController&&) = default;

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  bool AtReference() const;

  /**
   * Sets the pose error which is considered tolerable for use with
   * AtReference().
   *
   * @param tolerance Pose error which is tolerable.
   */
  void SetTolerance(const Pose2d& tolerance);

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
  ChassisSpeeds Calculate(const Pose2d& currentPose,
                          const Pose2d& trajectoryPose,
                          units::meters_per_second_t desiredLinearVelocity,
                          const Rotation2d& desiredHeading);

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
  ChassisSpeeds Calculate(const Pose2d& currentPose,
                          const Trajectory::State& desiredState,
                          const Rotation2d& desiredHeading);

  /**
   * Enables and disables the controller for troubleshooting purposes. When
   * Calculate() is called on a disabled controller, only feedforward values
   * are returned.
   *
   * @param enabled If the controller is enabled or not.
   */
  void SetEnabled(bool enabled);

  /**
   * Returns the rotation ProfiledPIDController
   */
  ProfiledPIDController<units::radian>& getThetaController();

  /**
   * Returns the X PIDController
   */
  PIDController& getXController();

  /**
   * Returns the Y PIDController
   */
  PIDController& getYController();

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
