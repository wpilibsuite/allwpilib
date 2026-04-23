// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import java.util.Optional;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisVelocities;

/**
 * {@code AntiTipping} provides a proportional correction system to prevent the robot from tipping
 * over during operation.
 *
 * <p>It uses pitch and roll measurements (in radians) to detect excessive inclination and computes
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

  private double m_inclinationMagnitude = 0.0;
  private Rotation2d m_fallDirection = Rotation2d.kZero;

  /**
   * Creates a new {@code AntiTipping} instance.
   *
   * @param kp The proportional coefficient.
   * @param tippingThreshold Tipping detection threshold (radians).
   * @param maxCorrectionSpeed Maximum correction velocity (meters per second).
   */
  public AntiTipping(double kp, double tippingThreshold, double maxCorrectionSpeed) {
    m_kp = kp;
    m_tippingThreshold = tippingThreshold;
    m_maxCorrectionSpeed = maxCorrectionSpeed;
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
   * Sets the Proportional coefficient.
   *
   * @param kp The proportional coefficient.
   */
  public void setP(double kp) {
    m_kp = kp;
  }

  /**
   * Gets the Proportional coefficient.
   *
   * @return The proportional coefficient.
   */
  public double getP() {
    return m_kp;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude Current robot attitude as a {@link Rotation3d}.
   * @return Correction {@link ChassisVelocities} to counteract tipping.
   */
  public Optional<ChassisVelocities> calculate(Rotation3d attitude) {
    double pitch = attitude.getY();
    double roll = attitude.getX();

    boolean isTipping = Math.abs(pitch) > m_tippingThreshold
        || Math.abs(roll) > m_tippingThreshold;

    /*
     * To find the exact fall direction, we calculate the projection of the 
     * robot's local Z-axis (up) onto the world XY plane.
     *
     * Assuming a standard Z-Y-X (Yaw-Pitch-Roll) rotation sequence, 
     * with Yaw=0 (since we want the fall direction relative to the chassis)
     *
     * R = Ry(pitch) * Rx(roll)
     *
     *     [ cos(p)   0   sin(p) ]   [ 1    0        0     ]
     * R = [   0      1     0    ] * [ 0  cos(r)  -sin(r)  ]
     *     [-sin(p)   0   cos(p) ]   [ 0  sin(r)   cos(r)  ]
     *
     * Multiplying the local Z vector [0, 0, 1]^T by R gives the tilted Z vector:
     *
     * [ sin(p) * cos(r) ]  <- X component (Forward fall)
     * Z_world_xy = [ -sin(r) ]  <- Y component (Left fall)
     * [ cos(p) * cos(r) ]
     *
     * We use these X and Y components to get the exact yaw direction of the fall, 
     * rather than relying on a small-angle approximation (where sin(θ) ≈ θ).
     */
    double xFall = Math.sin(pitch) * Math.cos(roll);
    double yFall = -Math.sin(roll);

    m_inclinationMagnitude = Math.hypot(xFall, yFall);

    if (m_inclinationMagnitude > 0.0) {
      m_fallDirection = new Rotation2d(xFall, yFall);

      Translation2d fallVector = new Translation2d(xFall, yFall).div(m_inclinationMagnitude);
      
      double correctionSpeed = 
          Math.clamp(m_kp * m_inclinationMagnitude, 0.0, m_maxCorrectionSpeed);
      Translation2d correctionVector = fallVector.times(correctionSpeed);

      if (isTipping) {
        return Optional.of(
            new ChassisVelocities(correctionVector.getX(), correctionVector.getY(), 0));
      }
    } else {
      m_fallDirection = Rotation2d.kZero;
    }

    return Optional.empty();
  }

  /**
   * Returns the latest tilt magnitude (hypotenuse of pitch and roll).
   *
   * @return The latest inclination magnitude in radians.
   */
  public double getLastInclinationMagnitude() {
    return m_inclinationMagnitude;
  }

  /**
   * Returns the most recent fall direction.
   *
   * @return The most recent fall direction as a {@link Rotation2d}.
   */
  public Rotation2d getLastFallDirection() {
    return m_fallDirection;
  }
}