// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <units/angular_velocity.h>
#include <units/velocity.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"

namespace frc {

/**
 * Ramsete is a nonlinear time-varying feedback controller for unicycle models
 * that drives the model to a desired pose along a two-dimensional trajectory.
 * Why would we need a nonlinear control law in addition to the linear ones we
 * have used so far like PID? If we use the original approach with PID
 * controllers for left and right position and velocity states, the controllers
 * only deal with the local pose. If the robot deviates from the path, there is
 * no way for the controllers to correct and the robot may not reach the desired
 * global pose. This is due to multiple endpoints existing for the robot which
 * have the same encoder path arc lengths.
 *
 * Instead of using wheel path arc lengths (which are in the robot's local
 * coordinate frame), nonlinear controllers like pure pursuit and Ramsete use
 * global pose. The controller uses this extra information to guide a linear
 * reference tracker like the PID controllers back in by adjusting the
 * references of the PID controllers.
 *
 * The paper "Control of Wheeled Mobile Robots: An Experimental Overview"
 * describes a nonlinear controller for a wheeled vehicle with unicycle-like
 * kinematics; a global pose consisting of x, y, and theta; and a desired pose
 * consisting of x_d, y_d, and theta_d. We call it Ramsete because that's the
 * acronym for the title of the book it came from in Italian ("Robotica
 * Articolata e Mobile per i SErvizi e le TEcnologie").
 *
 * See <https://file.tavsys.net/control/controls-engineering-in-frc.pdf> section
 * on Ramsete unicycle controller for a derivation and analysis.
 */
class RamseteController {
 public:
  /**
   * Construct a Ramsete unicycle controller.
   *
   * @param b    Tuning parameter (b > 0) for which larger values make
   *             convergence more aggressive like a proportional term.
   * @param zeta Tuning parameter (0 < zeta < 1) for which larger values provide
   *             more damping in response.
   */
  RamseteController(double b, double zeta);

  /**
   * Construct a Ramsete unicycle controller. The default arguments for
   * b and zeta of 2.0 and 0.7 have been well-tested to produce desirable
   * results.
   */
  RamseteController() : RamseteController(2.0, 0.7) {}

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
  void SetTolerance(const Pose2d& poseTolerance);

  /**
   * Returns the next output of the Ramsete controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose        The current pose.
   * @param poseRef            The desired pose.
   * @param linearVelocityRef  The desired linear velocity.
   * @param angularVelocityRef The desired angular velocity.
   */
  ChassisSpeeds Calculate(const Pose2d& currentPose, const Pose2d& poseRef,
                          units::meters_per_second_t linearVelocityRef,
                          units::radians_per_second_t angularVelocityRef);

  /**
   * Returns the next output of the Ramsete controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   */
  ChassisSpeeds Calculate(const Pose2d& currentPose,
                          const Trajectory::State& desiredState);

  /**
   * Enables and disables the controller for troubleshooting purposes.
   *
   * @param enabled If the controller is enabled or not.
   */
  void SetEnabled(bool enabled);

 private:
  double m_b;
  double m_zeta;

  Pose2d m_poseError;
  Pose2d m_poseTolerance;
  bool m_enabled = true;
};

}  // namespace frc
