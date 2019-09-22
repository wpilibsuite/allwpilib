/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/units.h>

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
class LTVUnicycleController {
 public:
  /**
   * Construct a linear time-varying unicycle controller.
   *
   * @param Qelems The maximum desired error tolerance for each state.
   * @param Relems The maximum desired control effort for each input.
   * @param dt     Discretization timestep.
   */
  LTVUnicycleController(const std::array<double, 3>& Qelems,
                        const std::array<double, 2>& Relems,
                        units::second_t dt);

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

 private:
  Eigen::Matrix<double, 2, 3> m_K0;
  Eigen::Matrix<double, 2, 3> m_K1;

  Pose2d m_poseError;
  Pose2d m_poseTolerance;
};

}  // namespace frc
