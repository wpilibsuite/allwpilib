// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angle.h>
#include <units/velocity.h>

#include "frc/controller/PIDController.h"
#include "frc/controller/ProfiledPIDController.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"

namespace frc {
/**
 * This holonomic drive controller can be used to follow trajectories using a
 * holonomic drive train (i.e. swerve or mecanum). Holonomic trajectory
 * following is a much simpler problem to solve compared to skid-steer style
 * drivetrains because it is possible to individually control forward, sideways,
 * and angular velocity.
 *
 * The holonomic drive controller takes in one PID controller for each
 * direction, forward and sideways, and one profiled PID controller for the
 * angular direction. Because the heading dynamics are decoupled from
 * translations, users can specify a custom heading that the drivetrain should
 * point toward. This heading reference is profiled for smoothness.
 */
class HolonomicDriveController {
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
      frc2::PIDController xController, frc2::PIDController yController,
      ProfiledPIDController<units::radian> thetaController);

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  bool AtReference() const;

  /**
   * Sets the pose error which is considered tolerable for use with
   * AtReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   */
  void SetTolerance(const Pose2d& tolerance);

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose        The current pose.
   * @param poseRef            The desired pose.
   * @param linearVelocityRef  The desired linear velocity.
   * @param angleRef           The desired ending angle.
   */
  ChassisSpeeds Calculate(const Pose2d& currentPose, const Pose2d& poseRef,
                          units::meters_per_second_t linearVelocityRef,
                          const Rotation2d& angleRef);

  /**
   * Returns the next output of the holonomic drive controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   * @param angleRef     The desired ending angle.
   */
  ChassisSpeeds Calculate(const Pose2d& currentPose,
                          const Trajectory::State& desiredState,
                          const Rotation2d& angleRef);

  /**
   * Enables and disables the controller for troubleshooting purposes. When
   * Calculate() is called on a disabled controller, only feedforward values
   * are returned.
   *
   * @param enabled If the controller is enabled or not.
   */
  void SetEnabled(bool enabled);

 private:
  Pose2d m_poseError;
  Pose2d m_poseTolerance;
  bool m_enabled = true;

  frc2::PIDController m_xController;
  frc2::PIDController m_yController;
  ProfiledPIDController<units::radian> m_thetaController;
};
}  // namespace frc
