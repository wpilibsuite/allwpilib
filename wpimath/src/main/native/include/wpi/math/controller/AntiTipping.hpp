// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gcem.hpp>

#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/base.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * AntiTipping provides a proportional correction system to prevent the robot
 * from tipping over during operation.
 *
 * <p>It uses pitch and roll measurements to detect excessive inclination and
 * computes a correction velocity in the opposite direction of the tilt. The
 * resulting correction can be added to the robot's translational velocity to
 * help stabilize it.
 *
 * <h2>Usage</h2>
 *
 * <ol>
 *   <li>Instantiate with initial configuration parameters.
 *   <li>Call Calculate() periodically (e.g. once per control loop).
 *   <li>Add the resulting correction to your drive command.
 * </ol>
 *
 * <h2>Tuning</h2>
 *
 * <p>All three parameters depend on the robot's center of gravity and
 * drivetrain, so they are best found empirically:
 *
 * <ul>
 *   <li>tippingThreshold: Drive the robot normally, including hard
 *       acceleration, braking, and turning, and record the largest pitch/roll
 *       magnitude observed. Set the threshold a few degrees above that worst
 *       case so normal driving does not trigger a correction, but well below
 *       the angle at which the robot actually tips.
 *   <li>kp: The correction speed is kp * sin(θ), so kp controls how
 *       aggressively the robot drives out from under a tilt. Start small and
 *       increase until recovery is brisk without overshooting or oscillating.
 *   <li>maxCorrectionSpeed: Cap the correction at a fraction of the
 *       drivetrain's maximum speed so the anti-tip response stays controllable
 *       and never overpowers the driver.
 * </ul>
 */
class WPILIB_DLLEXPORT AntiTipping {
 public:
  /// Proportional gain unit: meters per second per radian of inclination.
  using kp_unit =
      wpi::units::compound_unit<wpi::units::meters_per_second,
                                wpi::units::inverse<wpi::units::radians>>;

  /**
   * Creates a new AntiTipping instance.
   *
   * @param kp The proportional coefficient in meters per second per radian.
   *     The P controller input is the sine of the inclination angle, and the
   *     output is in meters per second.
   * @param tippingThreshold Tipping detection threshold.
   * @param maxCorrectionSpeed Maximum correction velocity.
   */
  constexpr AntiTipping(wpi::units::unit_t<kp_unit> kp,
                        wpi::units::radian_t tippingThreshold,
                        wpi::units::meters_per_second_t maxCorrectionSpeed)
      : m_kp{kp},
        m_tippingThreshold{tippingThreshold},
        m_maxCorrectionSpeed{maxCorrectionSpeed} {}

  /**
   * Sets the proportional coefficient.
   *
   * @param kp The proportional coefficient in meters per second per radian.
   */
  constexpr void SetP(wpi::units::unit_t<kp_unit> kp) { m_kp = kp; }

  /**
   * Gets the proportional coefficient.
   *
   * @return The proportional coefficient in meters per second per radian.
   */
  constexpr wpi::units::unit_t<kp_unit> GetP() const { return m_kp; }

  /**
   * Sets the tipping detection threshold.
   *
   * @param threshold The tipping threshold.
   */
  constexpr void SetTippingThreshold(wpi::units::radian_t threshold) {
    m_tippingThreshold = threshold;
  }

  /**
   * Gets the tipping detection threshold.
   *
   * @return The tipping threshold.
   */
  constexpr wpi::units::radian_t GetTippingThreshold() const {
    return m_tippingThreshold;
  }

  /**
   * Sets the maximum correction velocity.
   *
   * @param speed The maximum correction speed.
   */
  constexpr void SetMaxCorrectionSpeed(wpi::units::meters_per_second_t speed) {
    m_maxCorrectionSpeed = speed;
  }

  /**
   * Gets the maximum correction velocity.
   *
   * @return The maximum correction speed.
   */
  constexpr wpi::units::meters_per_second_t GetMaxCorrectionSpeed() const {
    return m_maxCorrectionSpeed;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude Current robot attitude as a Rotation3d.
   * @return Correction ChassisVelocities to counteract tipping. Returns zeros
   *     if below threshold.
   */
  constexpr ChassisVelocities Calculate(const Rotation3d& attitude) const {
    // To find the correction, we rotate the z axis (scaled by the P gain) by
    // the attitude, then project onto the x-y plane.
    Translation2d correction =
        Translation3d{0_m, 0_m, wpi::units::meter_t{m_kp.value()}}
            .RotateBy(attitude)
            .ToTranslation2d();
    wpi::units::meters_per_second_t speed{correction.Norm().value()};

    // Let inclination angle of 3D correction be θ.
    //
    //    _o_       +z
    //    \  |       ^
    //   h \θ|       |
    //      \|  +x <--
    //
    // where o is length of 2D correction and h is length of 3D correction.
    //
    //   sinθ = o/h
    //   θ = asin(speed / m_kp)
    wpi::units::radian_t inclinationAngle{
        gcem::asin(speed.value() / m_kp.value())};

    if (inclinationAngle < m_tippingThreshold) {
      return {};
    } else if (speed > m_maxCorrectionSpeed) {
      correction = correction * (m_maxCorrectionSpeed.value() / speed.value());
    }

    return {wpi::units::meters_per_second_t{correction.X().value()},
            wpi::units::meters_per_second_t{correction.Y().value()},
            0_rad_per_s};
  }

 private:
  wpi::units::unit_t<kp_unit> m_kp;
  wpi::units::radian_t m_tippingThreshold;
  wpi::units::meters_per_second_t m_maxCorrectionSpeed;
};

}  // namespace wpi::math
