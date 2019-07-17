/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

/**
 * A rotation in a 2d coordinate frame represented a point on the unit circle
 * (cosine and sine).
 */
public class Rotation2d {
  private double m_value;
  private double m_cos;
  private double m_sin;

  /**
   * Constructs a Rotation2d with a default angle of 0 degrees.
   */
  public Rotation2d() {
    m_value = 0.0;
    m_cos = 1.0;
    m_sin = 0.0;
  }

  /**
   * Constructs a Rotation2d with the given radian value.
   */
  public Rotation2d(double value) {
    m_value = value;
    m_cos = Math.cos(value);
    m_sin = Math.sin(value);
  }

  /**
   * Constructs a Rotation2d with the given x and y (cosine and sine)
   * components.
   */
  @SuppressWarnings("ParameterName")
  public Rotation2d(double x, double y) {
    double magnitude = Math.hypot(x, y);
    if (magnitude > 1e-6) {
      m_sin = y / magnitude;
      m_cos = x / magnitude;
    } else {
      m_sin = 0.0;
      m_cos = 1.0;
    }
    m_value = Math.atan2(m_sin, m_cos);
  }

  /**
   * Constructs and returns a Rotation2d with the given degree value.
   */
  static Rotation2d fromDegrees(double degrees) {
    return new Rotation2d(Math.toDegrees(degrees));
  }

  /**
   * Adds two rotations together, with the result being bounded between -kPi and
   * kPi.
   *
   * <p>For example, Rotation2d.fromDegrees(30) + Rotation2d.fromDegrees(60) =
   * Rotation2d{-kPi/2}
   */
  Rotation2d plus(Rotation2d other) {
    return rotateBy(other);
  }

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * <p>For example, Rotation2d.fromDegrees(10) - Rotation2d.fromDegrees(100) =
   * Rotation2d{-kPi/2}
   */
  Rotation2d minus(Rotation2d other) {
    return rotateBy(other.unaryMinus());
  }

  /**
   * Takes the inverse of the current rotation. This is simply the negative of
   * the current angular value.
   */
  Rotation2d unaryMinus() {
    return new Rotation2d(-m_value);
  }

  /**
   * Adds the new rotation to the current rotation using a rotation matrix.
   *
   * <p>The matrix multiplication is as follows:
   * [cos_new] = [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   * value_new = atan2(cos_new, sin_new)
   */
  Rotation2d rotateBy(Rotation2d other) {
    return new Rotation2d(
            m_cos * other.m_cos - m_sin * other.m_sin,
            m_cos * other.m_sin + m_sin * other.m_cos
    );
  }

  /*
   * Returns the radian value of the rotation.
   */
  double getRadians() {
    return m_value;
  }

  /**
   * Returns the degree value of the rotation.
   */
  double getDegrees() {
    return Math.toDegrees(m_value);
  }

  /**
   * Returns the cosine of the rotation.
   */
  double getCos() {
    return m_cos;
  }

  /**
   * Returns the sine of the rotation.
   */
  double getSin() {
    return m_sin;
  }

  /**
   * Returns the tangent of the rotation.
   */
  double getTan() {
    return m_sin / m_cos;
  }
}
