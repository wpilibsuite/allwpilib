// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.kinematics.ChassisVelocities;

/**
 * {@code AntiTipping} provides a proportional correction system to prevent the robot from tipping
 * over during operation.
 *
 * <p>It uses pitch and roll measurements to detect excessive inclination and computes
 * a correction velocity in the opposite direction of the tilt. The resulting correction can be
 * added to the robot’s translational velocity to help stabilize it.
 *
 * <h2>Usage</h2>
 * <ol>
 * <li>Instantiate with initial configuration parameters.
 * <li>Call {@link #calculate(Rotation3d)} periodically (e.g. once per control loop).
 * <li>Add the resulting correction to your drive command.
 * </ol>
 */
public class AntiTipping {
  private double m_tippingThreshold;
  private double m_maxCorrectionSpeed;
  private double m_kp;

  /**
   * Creates a new {@code AntiTipping} instance.
   *
   * @param kp The proportional coefficient in meters per second. The P controller input is the sine
   * of the inclination angle, and the output is in meters per second.
   * @param tippingThreshold Tipping detection threshold in radians.
   * @param maxCorrectionSpeed Maximum correction velocity in meters per second.
   */
  public AntiTipping(double kp, double tippingThreshold, double maxCorrectionSpeed) {
    m_kp = kp;
    m_tippingThreshold = tippingThreshold;
    m_maxCorrectionSpeed = maxCorrectionSpeed;
  }

  /**
   * Sets the Proportional coefficient.
   *
   * @param kp The proportional coefficient in meters per second.
   */
  public void setP(double kp) {
    m_kp = kp;
  }

  /**
   * Gets the Proportional coefficient.
   *
   * @return The proportional coefficient in meters per second.
   */
  public double getP() {
    return m_kp;
  }

  /**
   * Sets the tipping detection threshold.
   *
   * @param threshold The tipping threshold in radians.
   */
  public void setTippingThreshold(double threshold) {
    m_tippingThreshold = threshold;
  }

  /**
   * Gets the tipping detection threshold.
   *
   * @return The tipping threshold in radians.
   */
  public double getTippingThreshold() {
    return m_tippingThreshold;
  }

  /**
   * Sets the maximum correction velocity.
   *
   * @param speed The maximum correction speed in meters per second.
   */
  public void setMaxCorrectionSpeed(double speed) {
    m_maxCorrectionSpeed = speed;
  }

  /**
   * Gets the maximum correction velocity.
   *
   * @return The maximum correction speed in meters per second.
   */
  public double getMaxCorrectionSpeed() {
    return m_maxCorrectionSpeed;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude Current robot attitude as a {@link Rotation3d}.
   * @return Correction {@link ChassisVelocities} to counteract tipping. Returns zeros if below
   * threshold.
   */
  public ChassisVelocities calculate(Rotation3d attitude) {
    // To find the correction, we rotate the z axis (scaled by the P gain) by the attitude, then
    // project onto the x-y plane.
    var correction = new Translation3d(0.0, 0.0, m_kp).rotateBy(attitude).toTranslation2d();
    double norm = correction.getNorm();

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
    //   θ = asin(norm / m_kp)
    double inclinationAngle = Math.asin(norm / m_kp);

    if (inclinationAngle < m_tippingThreshold) {
      return new ChassisVelocities(0.0, 0.0, 0.0);
    } else if (norm > m_maxCorrectionSpeed) {
      // Clamp the correction to the maximum correction speed
      correction = correction.times(m_maxCorrectionSpeed / norm);
    }

    return new ChassisVelocities(correction.getX(), correction.getY(), 0.0);
  }
}