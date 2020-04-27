/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import java.util.Objects;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

import edu.wpi.first.math.interpolation.Interpolatable;
import edu.wpi.first.wpiutil.math.MathUtil;

/**
 * A rotation in a 2d coordinate frame represented a point on the unit circle
 * (cosine and sine).
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Rotation2d implements Interpolatable<Rotation2d> {
  private final double m_value;
  private final double m_cos;
  private final double m_sin;

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
   * The x and y don't have to be normalized.
   *
   * @param value The value of the angle in radians.
   */
  @JsonCreator
  public Rotation2d(@JsonProperty(required = true, value = "radians") double value) {
    m_value = value;
    m_cos = Math.cos(value);
    m_sin = Math.sin(value);
  }

  /**
   * Constructs a Rotation2d with the given x and y (cosine and sine)
   * components.
   *
   * @param x The x component or cosine of the rotation.
   * @param y The y component or sine of the rotation.
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
   *
   * @param degrees The value of the angle in degrees.
   * @return The rotation object with the desired angle value.
   */
  public static Rotation2d fromDegrees(double degrees) {
    return new Rotation2d(Math.toRadians(degrees));
  }

  /**
   * Adds two rotations together, with the result being bounded between -pi and
   * pi.
   *
   * <p>For example, Rotation2d.fromDegrees(30) + Rotation2d.fromDegrees(60) =
   * Rotation2d{-pi/2}
   *
   * @param other The rotation to add.
   * @return The sum of the two rotations.
   */
  public Rotation2d plus(Rotation2d other) {
    return rotateBy(other);
  }

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * <p>For example, Rotation2d.fromDegrees(10) - Rotation2d.fromDegrees(100) =
   * Rotation2d{-pi/2}
   *
   * @param other The rotation to subtract.
   * @return The difference between the two rotations.
   */
  public Rotation2d minus(Rotation2d other) {
    return rotateBy(other.unaryMinus());
  }

  /**
   * Takes the inverse of the current rotation. This is simply the negative of
   * the current angular value.
   *
   * @return The inverse of the current rotation.
   */
  public Rotation2d unaryMinus() {
    return new Rotation2d(-m_value);
  }

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   * @return The new scaled Rotation2d.
   */
  public Rotation2d times(double scalar) {
    return new Rotation2d(m_value * scalar);
  }

  /**
   * Adds the new rotation to the current rotation using a rotation matrix.
   *
   * <p>The matrix multiplication is as follows:
   * [cos_new]   [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   * value_new = atan2(cos_new, sin_new)
   *
   * @param other The rotation to rotate by.
   * @return The new rotated Rotation2d.
   */
  public Rotation2d rotateBy(Rotation2d other) {
    return new Rotation2d(
        m_cos * other.m_cos - m_sin * other.m_sin,
        m_cos * other.m_sin + m_sin * other.m_cos
    );
  }

  /**
   * Returns the radian value of the rotation.
   *
   * @return The radian value of the rotation.
   */
  @JsonProperty
  public double getRadians() {
    return m_value;
  }

  /**
   * Returns the degree value of the rotation.
   *
   * @return The degree value of the rotation.
   */
  public double getDegrees() {
    return Math.toDegrees(m_value);
  }

  /**
   * Returns the cosine of the rotation.
   *
   * @return The cosine of the rotation.
   */
  public double getCos() {
    return m_cos;
  }

  /**
   * Returns the sine of the rotation.
   *
   * @return The sine of the rotation.
   */
  public double getSin() {
    return m_sin;
  }

  /**
   * Returns the tangent of the rotation.
   *
   * @return The tangent of the rotation.
   */
  public double getTan() {
    return m_sin / m_cos;
  }

  @Override
  public String toString() {
    return String.format("Rotation2d(Rads: %.2f, Deg: %.2f)", m_value, Math.toDegrees(m_value));
  }

  /**
   * Checks equality between this Rotation2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Rotation2d) {
      return Math.abs(((Rotation2d) obj).m_value - m_value) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_value);
  }

  @Override
  @SuppressWarnings("ParameterName")
  public Rotation2d interpolate(Rotation2d endValue, double t) {
    return new Rotation2d(MathUtil.interpolate(this.getRadians(), endValue.getRadians(), t));
  }
}
