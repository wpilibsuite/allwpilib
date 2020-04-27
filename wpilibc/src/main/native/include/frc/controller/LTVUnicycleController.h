/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>

#include <Eigen/Core>
#include <units/angular_velocity.h>
#include <units/time.h>
#include <units/velocity.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"

namespace frc {

/**
 * A Linear Time-Varying Cascaded Unicycle Controller for differential drive
 * robots. Similar to RAMSETE, this controller combines feedback and feedforward
 * to output ChassisSpeeds to guide a robot along a trajectory. However, this
 * controller utilizes tolerances grounded in reality to pick gains rather than
 * magical Beta and Zeta gains.
 */
class LTVUnicycleController {
 public:
  /**
   * Construct a LTV Unicycle Controller.
   *
   * @param qElms     The maximum desired error tolerance for the robot's state,
   * in the form [X, Y, Heading]^T. Units are meters and radians.
   * @param rElms     The maximum desired control effort by the feedback
   * controller, in the form [vMax, wMax]^T. Units are meters per second and
   *                  radians per second. Note that this is not the maximum
   * speed of the robot, but rather the maximum effort the feedback controller
   *                  should apply on top of the trajectory feedforward.
   * @param dtSeconds The nominal dt of this controller. With command based this
   * is 0.020.
   */
  LTVUnicycleController(const std::array<double, 3>& Qelems,
                        const std::array<double, 2>& Relems,
                        units::second_t dt);

  /**
   * Construct a LTV Unicycle Controller.
   *
   * @param qElms     The maximum desired error tolerance for the robot's state,
   * in the form [X, Y, Heading]^T. Units are meters and radians.
   * @param rho       A weighting factor that balances control effort and state
   * excursion. Greater values penalize state excursion more heavily. 1 is a
   * good starting value.
   * @param rElms     The maximum desired control effort by the feedback
   * controller, in the form [vMax, wMax]^T. Units are meters per second and
   *                  radians per second. Note that this is not the maximum
   * speed of the robot, but rather the maximum effort the feedback controller
   *                  should apply on top of the trajectory feedforward.
   * @param dtSeconds The nominal dt of this controller. With command based this
   * is 0.020.
   */
  LTVUnicycleController(const std::array<double, 3>& Qelems, const double rho,
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
   * Returns the next output of the LTV Unicycle Controller.
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
   * Returns the next output of the LTV Unicycle Controller.
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
