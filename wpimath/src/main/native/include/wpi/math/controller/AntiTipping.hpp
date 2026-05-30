// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
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
  /**
   * Creates a new AntiTipping instance.
   *
   * @param kp The proportional coefficient in meters per second. The P
   *     controller input is the sine of the inclination angle, and the output
   *     is in meters per second.
   * @param tippingThreshold Tipping detection threshold in radians.
   * @param maxCorrectionSpeed Maximum correction velocity in meters per second.
   */
  constexpr AntiTipping(double kp, double tippingThreshold,
                        double maxCorrectionSpeed)
      : m_kp(kp),
        m_tippingThreshold(tippingThreshold),
        m_maxCorrectionSpeed(maxCorrectionSpeed) {}

  /**
   * Sets the proportional coefficient.
   *
   * @param kp The proportional coefficient in meters per second.
   */
  constexpr void SetP(double kp) { m_kp = kp; }

  /**
   * Gets the proportional coefficient.
   *
   * @return The proportional coefficient in meters per second.
   */
  constexpr double GetP() const { return m_kp; }

  /**
   * Sets the tipping detection threshold.
   *
   * @param threshold The tipping threshold in radians.
   */
  constexpr void SetTippingThreshold(double threshold) {
    m_tippingThreshold = threshold;
  }

  /**
   * Gets the tipping detection threshold.
   *
   * @return The tipping threshold in radians.
   */
  constexpr double GetTippingThreshold() const { return m_tippingThreshold; }

  /**
   * Sets the maximum correction velocity.
   *
   * @param speed The maximum correction speed in meters per second.
   */
  constexpr void SetMaxCorrectionSpeed(double speed) {
    m_maxCorrectionSpeed = speed;
  }

  /**
   * Gets the maximum correction velocity.
   *
   * @return The maximum correction speed in meters per second.
   */
  constexpr double GetMaxCorrectionSpeed() const {
    return m_maxCorrectionSpeed;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude Current robot attitude as a Rotation3d.
   * @return Correction ChassisVelocities to counteract tipping. Returns zeros
   *     if below threshold.
   */
  ChassisVelocities Calculate(const Rotation3d& attitude);

 private:
  double m_kp;
  double m_tippingThreshold;
  double m_maxCorrectionSpeed;
};

}  // namespace wpi::math
