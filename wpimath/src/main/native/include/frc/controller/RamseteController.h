// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/deprecated.h>

#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/trajectory/Trajectory.h"
#include "units/angle.h"
#include "units/angular_velocity.h"
#include "units/length.h"
#include "units/math.h"
#include "units/velocity.h"

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
class WPILIB_DLLEXPORT RamseteController {
 public:
  using b_unit =
      units::compound_unit<units::squared<units::radians>,
                           units::inverse<units::squared<units::meters>>>;
  using zeta_unit = units::inverse<units::radians>;

  /**
   * Construct a Ramsete unicycle controller.
   *
   * @param b    Tuning parameter (b > 0 rad²/m²) for which larger values make
   *             convergence more aggressive like a proportional term.
   * @param zeta Tuning parameter (0 rad⁻¹ < zeta < 1 rad⁻¹) for which larger
   *             values provide more damping in response.
   * @deprecated Use LTVUnicycleController instead.
   */
  [[deprecated("Use LTVUnicycleController instead.")]]
  constexpr RamseteController(units::unit_t<b_unit> b,
                              units::unit_t<zeta_unit> zeta)
      : m_b{b}, m_zeta{zeta} {}

  WPI_IGNORE_DEPRECATED

  /**
   * Construct a Ramsete unicycle controller. The default arguments for
   * b and zeta of 2.0 rad²/m² and 0.7 rad⁻¹ have been well-tested to produce
   * desirable results.
   *
   * @deprecated Use LTVUnicycleController instead.
   */
  [[deprecated("Use LTVUnicycleController instead.")]]
  constexpr RamseteController()
      : RamseteController{units::unit_t<b_unit>{2.0},
                          units::unit_t<zeta_unit>{0.7}} {}

  WPI_UNIGNORE_DEPRECATED

  /**
   * Returns true if the pose error is within tolerance of the reference.
   */
  constexpr bool AtReference() const {
    const auto& eTranslate = m_poseError.Translation();
    const auto& eRotate = m_poseError.Rotation();
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
   * @param poseTolerance Pose error which is tolerable.
   */
  constexpr void SetTolerance(const Pose2d& poseTolerance) {
    m_poseTolerance = poseTolerance;
  }

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
  constexpr ChassisSpeeds Calculate(
      const Pose2d& currentPose, const Pose2d& poseRef,
      units::meters_per_second_t linearVelocityRef,
      units::radians_per_second_t angularVelocityRef) {
    if (!m_enabled) {
      return ChassisSpeeds{linearVelocityRef, 0_mps, angularVelocityRef};
    }

    m_poseError = poseRef.RelativeTo(currentPose);

    // Aliases for equation readability
    const auto& eX = m_poseError.X();
    const auto& eY = m_poseError.Y();
    const auto& eTheta = m_poseError.Rotation().Radians();
    const auto& vRef = linearVelocityRef;
    const auto& omegaRef = angularVelocityRef;

    // k = 2ζ√(ω_ref² + b v_ref²)
    auto k = 2.0 * m_zeta *
             units::math::sqrt(units::math::pow<2>(omegaRef) +
                               m_b * units::math::pow<2>(vRef));

    // v_cmd = v_ref cos(e_θ) + k e_x
    // ω_cmd = ω_ref + k e_θ + b v_ref sinc(e_θ) e_y
    return ChassisSpeeds{
        vRef * m_poseError.Rotation().Cos() + k * eX, 0_mps,
        omegaRef + k * eTheta + m_b * vRef * Sinc(eTheta) * eY};
  }

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
  constexpr ChassisSpeeds Calculate(const Pose2d& currentPose,
                                    const Trajectory::State& desiredState) {
    return Calculate(currentPose, desiredState.pose, desiredState.velocity,
                     desiredState.velocity * desiredState.curvature);
  }

  /**
   * Enables and disables the controller for troubleshooting purposes.
   *
   * @param enabled If the controller is enabled or not.
   */
  constexpr void SetEnabled(bool enabled) { m_enabled = enabled; }

 private:
  units::unit_t<b_unit> m_b;
  units::unit_t<zeta_unit> m_zeta;

  Pose2d m_poseError;
  Pose2d m_poseTolerance;
  bool m_enabled = true;

  /**
   * Returns sin(x) / x.
   *
   * @param x Value of which to take sinc(x).
   */
  static constexpr decltype(1 / 1_rad) Sinc(units::radian_t x) {
    if (units::math::abs(x) < 1e-9_rad) {
      return decltype(1 / 1_rad){1.0 - 1.0 / 6.0 * x.value() * x.value()};
    } else {
      return units::math::sin(x) / x;
    }
  }
};

}  // namespace frc
