package edu.wpi.first.math.controller;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import java.util.Optional;

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
  private double m_P;

  private double m_inclinationMagnitude = 0.0;
  private double m_yawDirection = 0.0;
  private Rotation2d m_tiltDirection = new Rotation2d();

  /**
   * Creates a new {@code AntiTipping} instance.
   *
   * @param P proportional gain for correction
   * @param tippingThreshold tipping detection threshold (radians)
   * @param maxCorrectionSpeed maximum correction velocity (meters per second)
   */
  public AntiTipping(double P, double tippingThreshold, double maxCorrectionSpeed) {
    m_P = P;
    m_tippingThreshold = tippingThreshold;
    m_maxCorrectionSpeed = maxCorrectionSpeed;
  }

  /**
   * Sets the tipping detection threshold.
   *
   * @param threshold the tipping threshold in radians
   */
  public void setTippingThreshold(double threshold) {
    m_tippingThreshold = threshold;
  }

  /**
   * Gets the tipping detection threshold.
   *
   * @return the tipping threshold in radians
   */
  public double getTippingThreshold() {
    return m_tippingThreshold;
  }

  /**
   * Sets the maximum correction velocity.
   *
   * @param speed the maximum correction speed in meters per second
   */
  public void setMaxCorrectionSpeed(double speed) {
    m_maxCorrectionSpeed = speed;
  }

  /**
   * Gets the maximum correction velocity.
   *
   * @return the maximum correction speed in meters per second
   */
  public double getMaxCorrectionSpeed() {
    return m_maxCorrectionSpeed;
  }

  /**
   * Sets the proportional gain for correction.
   *
   * @param P proportional gain
   */
  public void setP(double P) {
    m_P = P;
  }

  /**
   * Gets the proportional gain for correction.
   *
   * @return the proportional gain
   */
  public double getP() {
    return m_P;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude current robot attitude as a {@link Rotation3d}
   * @return correction {@link ChassisSpeeds} to counteract tipping
   */
  public Optional<ChassisSpeeds> calculate(Rotation3d attitude) {
    double pitch = attitude.getY();
    double roll = attitude.getX();

    boolean isTipping = Math.abs(pitch) > m_tippingThreshold
        || Math.abs(roll) > m_tippingThreshold;

    m_inclinationMagnitude = Math.hypot(pitch, roll);

    if (m_inclinationMagnitude > 0.0) {
      m_tiltDirection = new Rotation2d(pitch, -roll);
      m_yawDirection = m_tiltDirection.getRadians();

      Translation2d fallDirection = new Translation2d(pitch, -roll).div(m_inclinationMagnitude);
      
      double correctionSpeed = Math.clamp(m_P * m_inclinationMagnitude, 0.0, m_maxCorrectionSpeed);
      Translation2d correctionVector = fallDirection.times(correctionSpeed);

      if (isTipping) {
        return Optional.of(new ChassisSpeeds(correctionVector.getX(), correctionVector.getY(), 0));
      }
    } else {
      m_tiltDirection = new Rotation2d();
      m_yawDirection = 0.0;
    }

    return Optional.empty();
  }

  /**
   * Returns the latest tilt magnitude (hypotenuse of pitch and roll).
   *
   * @return the latest inclination magnitude in radians
   */
  public double getLastInclinationMagnitude() {
    return m_inclinationMagnitude;
  }

  /**
   * Returns the most recent tilt direction (pseudo-yaw).
   *
   * @return the most recent yaw direction in radians
   */
  public double getLastYawDirection() {
    return m_yawDirection;
  }

  /**
   * Returns the most recent tilt direction as a {@link Rotation2d}.
   *
   * @return the most recent tilt direction
   */
  public Rotation2d getLastTiltDirection() {
    return m_tiltDirection;
  }
}