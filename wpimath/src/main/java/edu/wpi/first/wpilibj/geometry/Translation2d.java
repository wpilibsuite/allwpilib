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
 * Represents a translation in 2d space.
 * This object can be used to represent a point or a vector.
 *
 * <p>This assumes that you are using conventional mathematical axes.
 * When the robot is placed on the origin, facing toward the X direction,
 * moving forward increases the X, whereas moving to the left increases the Y.
 */
@SuppressWarnings({"ParameterName", "MemberName"})
@JsonIgnoreProperties(ignoreUnknown = true)
@JsonAutoDetect(getterVisibility = JsonAutoDetect.Visibility.NONE)
public class Translation2d implements Interpolatable<Translation2d> {
  private final double m_x;
  private final double m_y;

  /**
   * Constructs a Translation2d with X and Y components equal to zero.
   */
  public Translation2d() {
    this(0.0, 0.0);
  }

  /**
   * Constructs a Translation2d with the X and Y components equal to the
   * provided values.
   *
   * @param x The x component of the translation.
   * @param y The y component of the translation.
   */
  @JsonCreator
  public Translation2d(@JsonProperty(required = true, value = "x") double x,
                       @JsonProperty(required = true, value = "y") double y) {
    m_x = x;
    m_y = y;
  }

  /**
   * Calculates the distance between two translations in 2d space.
   *
   * <p>This function uses the pythagorean theorem to calculate the distance.
   * distance = sqrt((x2 - x1)^2 + (y2 - y1)^2)
   *
   * @param other The translation to compute the distance to.
   * @return The distance between the two translations.
   */
  public double getDistance(Translation2d other) {
    return Math.hypot(other.m_x - m_x, other.m_y - m_y);
  }

  /**
   * Returns the X component of the translation.
   *
   * @return The x component of the translation.
   */
  @JsonProperty
  public double getX() {
    return m_x;
  }

  /**
   * Returns the Y component of the translation.
   *
   * @return The y component of the translation.
   */
  @JsonProperty
  public double getY() {
    return m_y;
  }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  public double getNorm() {
    return Math.hypot(m_x, m_y);
  }

  /**
   * Applies a rotation to the translation in 2d space.
   *
   * <p>This multiplies the translation vector by a counterclockwise rotation
   * matrix of the given angle.
   * [x_new]   [other.cos, -other.sin][x]
   * [y_new] = [other.sin,  other.cos][y]
   *
   * <p>For example, rotating a Translation2d of {2, 0} by 90 degrees will return a
   * Translation2d of {0, 2}.
   *
   * @param other The rotation to rotate the translation by.
   * @return The new rotated translation.
   */
  public Translation2d rotateBy(Rotation2d other) {
    return new Translation2d(
            m_x * other.getCos() - m_y * other.getSin(),
            m_x * other.getSin() + m_y * other.getCos()
    );
  }

  /**
   * Adds two translations in 2d space and returns the sum. This is similar to
   * vector addition.
   *
   * <p>For example, Translation2d{1.0, 2.5} + Translation2d{2.0, 5.5} =
   * Translation2d{3.0, 8.0}
   *
   * @param other The translation to add.
   * @return The sum of the translations.
   */
  public Translation2d plus(Translation2d other) {
    return new Translation2d(m_x + other.m_x, m_y + other.m_y);
  }

  /**
   * Subtracts the other translation from the other translation and returns the
   * difference.
   *
   * <p>For example, Translation2d{5.0, 4.0} - Translation2d{1.0, 2.0} =
   * Translation2d{4.0, 2.0}
   *
   * @param other The translation to subtract.
   * @return The difference between the two translations.
   */
  public Translation2d minus(Translation2d other) {
    return new Translation2d(m_x - other.m_x, m_y - other.m_y);
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or simply
   * negating both components of the translation.
   *
   * @return The inverse of the current translation.
   */
  public Translation2d unaryMinus() {
    return new Translation2d(-m_x, -m_y);
  }

  /**
   * Multiplies the translation by a scalar and returns the new translation.
   *
   * <p>For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled translation.
   */
  public Translation2d times(double scalar) {
    return new Translation2d(m_x * scalar, m_y * scalar);
  }

  /**
   * Divides the translation by a scalar and returns the new translation.
   *
   * <p>For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}
   *
   * @param scalar The scalar to multiply by.
   * @return The reference to the new mutated object.
   */
  public Translation2d div(double scalar) {
    return new Translation2d(m_x / scalar, m_y / scalar);
  }

  @Override
  public String toString() {
    return String.format("Translation2d(X: %.2f, Y: %.2f)", m_x, m_y);
  }

  /**
   * Checks equality between this Translation2d and another object.
   *
   * @param obj The other object.
   * @return Whether the two objects are equal or not.
   */
  @Override
  public boolean equals(Object obj) {
    if (obj instanceof Translation2d) {
      return Math.abs(((Translation2d) obj).m_x - m_x) < 1E-9
          && Math.abs(((Translation2d) obj).m_y - m_y) < 1E-9;
    }
    return false;
  }

  @Override
  public int hashCode() {
    return Objects.hash(m_x, m_y);
  }

  @Override
  public Translation2d interpolate(Translation2d endValue, double t) {
    return new Translation2d(MathUtil.interpolate(this.getX(), endValue.getX(), t),
        MathUtil.interpolate(this.getY(), endValue.getY(), t));
  }
}
