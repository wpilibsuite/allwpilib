// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.units.measure.Angle;

/**
 * A rotation in a 2D coordinate frame represented by a point on the unit circle (cosine and sine).
 *
 * <p>The angle is continuous, that is if a Rotation2d is constructed with 361 degrees, it will
 * return 361 degrees. This allows algorithms that wouldn't want to see a discontinuity in the
 * rotations as it sweeps past from 360 to 0 on the second time around.
 */
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class MutRotation2d extends Rotation2d {
  /** Constructs a MutRotation2d with a default angle of 0 degrees. */
  public MutRotation2d() {
    super();
  }

  /**
   * Constructs a MutRotation2d with the given radian value.
   *
   * @param value The value of the angle in radians.
   */
  @JsonCreator
  public MutRotation2d(@JsonProperty(required = true, value = "radians") double value) {
    super(value);
  }

  /**
   * Constructs a MutRotation2d with the given x and y (cosine and sine) components.
   *
   * @param x The x component or cosine of the rotation.
   * @param y The y component or sine of the rotation.
   */
  public MutRotation2d(double x, double y) {
    super(x, y);
  }

  /**
   * Constructs a MutRotation2d with the given angle.
   *
   * @param angle The angle of the rotation.
   */
  public MutRotation2d(Angle angle) {
    super(angle);
  }

  /**
   * sets the value of this MutRotation2d to the given radian value.
   *
   * @param radians The new value of this MutRotation2d in radians.
   */
  public void mut_fromRadians(double radians) {
    m_value = radians;
    m_cos = Math.cos(radians);
    m_sin = Math.sin(radians);
  }

  /**
   * sets the value of this MutRotation2d to the given degrees value.
   *
   * @param degrees The new value of this MutRotation2d in degrees.
   */
  public void mut_fromDegrees(double degrees) {
    mut_fromRadians(Math.toRadians(degrees));
  }

  /**
   * sets the value of this MutRotation2d to the given rotations value.
   *
   * @param rotations The new value of this MutRotation2d in rotations.
   */
  public void mut_fromRotations(double rotations) {
    mut_fromRadians(Units.rotationsToRadians(rotations));
  }

  /**
   * sets the value of this MutRotation2d to the given Rotation2d value.
   *
   * @param other The new value of this MutRotation2d.
   */
  public void mut_Rotation2d(Rotation2d other) {
    mut_fromRadians(other.getRadians());
  }

  /**
   * Adds the new rotation to this one using a rotation matrix.
   *
   * <p>The matrix multiplication is as follows:
   *
   * <pre>
   * [cos_new]   [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   * value_new = atan2(sin_new, cos_new)
   * </pre>
   *
   * @param other The rotation to rotate by.
   */
  public void mut_rotateBy(Rotation2d other) {
    double x = m_cos * other.m_cos - m_sin * other.m_sin;
    double y = m_cos * other.m_sin + m_sin * other.m_cos;
    double magnitude = Math.hypot(x, y);
    if (magnitude > 1e-6) {
      m_sin = y / magnitude;
      m_cos = x / magnitude;
    } else {
      m_sin = 0.0;
      m_cos = 1.0;
      MathSharedStore.reportError(
          "x and y components of MutRotation2d are zero\n", Thread.currentThread().getStackTrace());
    }
    m_value = Math.atan2(m_sin, m_cos);
  }

  /**
   * Adds the other rotation to this one, with the result being bounded between -pi and pi.
   *
   * @param other The rotation to add.
   */
  public void mut_plus(Rotation2d other) {
    mut_rotateBy(other);
  }

  /**
   * Subtracts the other rotation from this one.
   *
   * @param other The rotation to subtract.
   */
  public void mut_minus(MutRotation2d other) {
    double x = m_cos * other.m_cos + m_sin * other.m_sin;
    double y = -m_cos * other.m_sin + m_sin * other.m_cos;
    double magnitude = Math.hypot(x, y);
    if (magnitude > 1e-6) {
      m_sin = y / magnitude;
      m_cos = x / magnitude;
    } else {
      m_sin = 0.0;
      m_cos = 1.0;
      MathSharedStore.reportError(
          "x and y components of MutRotation2d are zero\n", Thread.currentThread().getStackTrace());
    }
    m_value = Math.atan2(m_sin, m_cos);
  }

  /** Inverts this rotation. This is simply the negative of the current angular value. */
  public void mut_unaryMinus() {
    m_value *= -1;
    m_cos = Math.cos(m_value);
    m_sin = Math.sin(m_value);
  }

  /**
   * Multiplies this rotation by a scalar.
   *
   * @param scalar The scalar.
   */
  public void mut_times(double scalar) {
    m_value *= scalar;
    m_cos = Math.cos(m_value);
    m_sin = Math.sin(m_value);
  }

  /**
   * Divides this rotation by a scalar.
   *
   * @param scalar The scalar.
   */
  public void mut_div(double scalar) {
    mut_times(1.0 / scalar);
  }

  /**
   * Determines this delta in this rotation and other in radians.
   *
   * @param other The other rotation.
   * @return The delta in radians.
   */
  public double deltaRadians(Rotation2d other) {
    double x = m_cos * other.m_cos + m_sin * other.m_sin;
    double y = -m_cos * other.m_sin + m_sin * other.m_cos;
    double magnitude = Math.hypot(x, y);
    if (magnitude > 1e-6) {
      y /= magnitude;
      x /= magnitude;
    } else {
      y = 0.0;
      x = 1.0;
      MathSharedStore.reportError(
          "x and y components of MutRotation2d are zero\n", Thread.currentThread().getStackTrace());
    }
    return Math.atan2(y, x);
  }

  /**
   * Determines this delta in this rotation and other in degrees.
   *
   * @param other The other rotation.
   * @return The delta in degrees.
   */
  public double deltaDegrees(Rotation2d other) {
    return Math.toDegrees(deltaRadians(other));
  }

  /**
   * Determines this delta in this rotation and other in rotations.
   *
   * @param other The other rotation.
   * @return The delta in rotations.
   */
  public double deltaRotations(Rotation2d other) {
    return Units.radiansToRotations(deltaRadians(other));
  }

  @Override
  public String toString() {
    return String.format("MutRotation2d(Rads: %.2f, Deg: %.2f)", m_value, Math.toDegrees(m_value));
  }
}
