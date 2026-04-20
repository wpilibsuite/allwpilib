package edu.wpi.first.math.controller;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import java.util.Optional;

/**
 * {@code AntiTipping} provides a proportional correction system to prevent the robot from tipping
 * over during operation.
 *
 * <p>It uses pitch and roll measurements (in degrees) to detect excessive inclination and computes
 * a correction velocity in the opposite direction of the tilt. The resulting correction can be
 * added to the robot’s translational velocity to help stabilize it.
 *
 * <h2>Usage</h2>
 * <ol>
 * <li>Instantiate with initial configuration parameters.
 * <li>Call {@link #calculate(Rotation3d)} periodically (e.g. once per control loop).
 * <li>Add the resulting correction to your drive command.
 * </ol>
 *
 * @since 2027
 */
public class AntiTipping {

  private double m_tippingThresholdDegrees;
  private double m_maxCorrectionSpeed;
  private double m_kP;

  private double m_inclinationMagnitude = 0.0;
  private double m_yawDirectionDeg = 0.0;
  private Rotation2d m_tiltDirection = new Rotation2d();

  /**
   * Creates a new {@code AntiTipping} instance.
   *
   * @param kP proportional gain for correction
   * @param tippingThresholdDegrees tipping detection threshold (degrees)
   * @param maxCorrectionSpeed maximum correction velocity (m/s)
   */
  public AntiTipping(double kP, double tippingThresholdDegrees, double maxCorrectionSpeed) {
    m_kP = kP;
    m_tippingThresholdDegrees = tippingThresholdDegrees;
    m_maxCorrectionSpeed = maxCorrectionSpeed;
  }

  public double getTippingThreshold() {
    return m_tippingThresholdDegrees;
  }

  public void setTippingThreshold(double degrees) {
    m_tippingThresholdDegrees = degrees;
  }

  public double getMaxCorrectionSpeed() {
    return m_maxCorrectionSpeed;
  }

  public void setMaxCorrectionSpeed(double speedMetersPerSecond) {
    m_maxCorrectionSpeed = speedMetersPerSecond;
  }

  public double getKP() {
    return m_kP;
  }

  public void setKP(double kP) {
    m_kP = kP;
  }

  /**
   * Updates tipping detection and computes the proportional correction.
   *
   * @param attitude current robot attitude as a {@link Rotation3d}
   * @return correction {@link ChassisSpeeds} to counteract tipping
   */
  public Optional<ChassisSpeeds> calculate(Rotation3d attitude) {
    double pitchDegrees = Math.toDegrees(attitude.getY());
    double rollDegrees = Math.toDegrees(attitude.getX());

    boolean isTipping = Math.abs(pitchDegrees) > m_tippingThresholdDegrees
        || Math.abs(rollDegrees) > m_tippingThresholdDegrees;

    m_inclinationMagnitude = Math.hypot(pitchDegrees, rollDegrees);

    if (m_inclinationMagnitude > 0.0) {
      m_tiltDirection = new Rotation2d(pitchDegrees, -rollDegrees);
      m_yawDirectionDeg = m_tiltDirection.getDegrees();

      Translation2d fallDirection = new Translation2d(pitchDegrees, -rollDegrees).div(m_inclinationMagnitude);
      
      double correctionSpeed = MathUtil.clamp(m_kP * m_inclinationMagnitude, 0.0, m_maxCorrectionSpeed);
      Translation2d correctionVector = fallDirection.times(correctionSpeed);

      if (isTipping) {
        return Optional.of(new ChassisSpeeds(correctionVector.getX(), correctionVector.getY(), 0));
      }
    } else {
      m_tiltDirection = new Rotation2d();
      m_yawDirectionDeg = 0.0;
    }

    return Optional.empty();
  }

  public double getLastInclinationMagnitude() {
    return m_inclinationMagnitude;
  }

  public double getLastYawDirectionDeg() {
    return m_yawDirectionDeg;
  }

  public Rotation2d getLastTiltDirection() {
    return m_tiltDirection;
  }
}