// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/math/util/StateSpaceUtil.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {

/**
 * The linear time-varying unicycle controller has a similar form to the LQR,
 * but the model used to compute the controller gain is the nonlinear unicycle
 * model linearized around the drivetrain's current state.
 *
 * See section 8.9 in Controls Engineering in FRC for a derivation of the
 * control law we used shown in theorem 8.9.1.
 */
class WPILIB_DLLEXPORT LTVUnicycleController {
 public:
  /**
   * Constructs a linear time-varying unicycle controller with default maximum
   * desired error tolerances of (x = 0.0625 m, y = 0.125 m, heading = 2 rad)
   * and default maximum desired control effort of (linear velocity = 1 m/s,
   * angular velocity = 2 rad/s).
   *
   * @param dt Discretization timestep.
   */
  explicit LTVUnicycleController(wpi::units::second_t dt)
      : LTVUnicycleController{{0.0625, 0.125, 2.0}, {1.0, 2.0}, dt} {}

  /**
   * Constructs a linear time-varying unicycle controller.
   *
   * See
   * https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-intro.html#lqr-tuning
   * for how to select the tolerances.
   *
   * @param Qelems The maximum desired error tolerance for each state (x, y,
   *               heading).
   * @param Relems The maximum desired control effort for each input (linear
   *               velocity, angular velocity).
   * @param dt     Discretization timestep.
   */
  LTVUnicycleController(const wpi::util::array<double, 3>& Qelems,
                        const wpi::util::array<double, 2>& Relems,
                        wpi::units::second_t dt)
      : m_Q{wpi::math::CostMatrix(Qelems)},
        m_R{wpi::math::CostMatrix(Relems)},
        m_dt{dt} {}

  /**
   * Move constructor.
   */
  LTVUnicycleController(LTVUnicycleController&&) = default;

  /**
   * Move assignment operator.
   */
  LTVUnicycleController& operator=(LTVUnicycleController&&) = default;

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  bool AtReference() const {
    const auto& eTranslate = m_poseError.Translation();
    const auto& eRotate = m_poseError.Rotation();
    const auto& tolTranslate = m_poseTolerance.Translation();
    const auto& tolRotate = m_poseTolerance.Rotation();
    return wpi::units::math::abs(eTranslate.X()) < tolTranslate.X() &&
           wpi::units::math::abs(eTranslate.Y()) < tolTranslate.Y() &&
           wpi::units::math::abs(eRotate.Radians()) < tolRotate.Radians();
  }

  /**
   * Sets the pose error which is considered tolerable for use with
   * AtReference().
   *
   * @param poseTolerance Pose error which is tolerable.
   */
  void SetTolerance(const Pose2d& poseTolerance) {
    m_poseTolerance = poseTolerance;
  }

  /**
   * Returns the linear and angular velocity outputs of the LTV controller.
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
                          wpi::units::meters_per_second_t linearVelocityRef,
                          wpi::units::radians_per_second_t angularVelocityRef);

  /**
   * Returns the linear and angular velocity outputs of the LTV controller.
   *
   * The reference pose, linear velocity, and angular velocity should come from
   * a drivetrain trajectory.
   *
   * @param currentPose  The current pose.
   * @param desiredState The desired pose, linear velocity, and angular velocity
   *                     from a trajectory.
   */
  ChassisSpeeds Calculate(const Pose2d& currentPose,
                          const Trajectory::State& desiredState) {
    return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                     desiredState.velocity * desiredState.curvature);
  }

  /**
   * Enables and disables the controller for troubleshooting purposes.
   *
   * @param enabled If the controller is enabled or not.
   */
  void SetEnabled(bool enabled) { m_enabled = enabled; }

 private:
  // LQR cost matrices
  Eigen::Matrix<double, 3, 3> m_Q;
  Eigen::Matrix<double, 2, 2> m_R;

  wpi::units::second_t m_dt;

  Pose2d m_poseError;
  Pose2d m_poseTolerance;
  bool m_enabled = true;
};

}  // namespace wpi::math
