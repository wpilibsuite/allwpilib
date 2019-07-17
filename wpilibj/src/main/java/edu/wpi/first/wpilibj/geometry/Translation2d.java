/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

/**
 * Represents a translation in 2d space.
 * This object can be used to represent a point or a vector.
 * <p>
 * This assumes that you are using conventional mathematical axes.
 * When the robot is placed on the origin, facing toward the X direction,
 * moving forward increases the X, whereas moving to the left increases the Y.
 */
public class Translation2d {
  private double m_x;
  private double m_y;

  /**
   * Constructs a Translation2d with X and Y components equal to zero.
   */
  public Translation2d() {
  }

  /**
   * Constructs a Translation2d with the X and Y components equal to the
   * provided values.
   */
  public Translation2d(double x, double y) {
    m_x = x;
    m_y = y;
  }

  /**
   * Calculates the distance between two translations in 2d space.
   * <p>
   * This function uses the pythagorean theorem to calculate the distance.
   * distance = sqrt((x2 - x1)^2 + (y2 - y1)^2)
   */
  double getDistance(Translation2d other) {
    return Math.hypot(other.m_x - m_x, other.m_y - m_y);
  }

  /*
   * Returns the X component of the translation.
   */
  double getX() {
    return m_x;
  }

  /*
   * Returns the Y component of the translation.
   */
  double getY() {
    return m_y;
  }

  /*
   * Returns the norm, or distance from the origin to the translation.
   */
  double getNorm() {
    return Math.hypot(m_x, m_y);
  }

  /*
   * Apply a rotation to the translation in 2d space.
   *
   * This multiplies the translation vector by a counterclockwise rotation
   * matrix of the given angle.
   *
   * [x_new] = [other.cos, -other.sin][x]
   * [y_new] = [other.sin,  other.cos][y]
   *
   * For example, rotating a Translation2d of {2, 0} by 90 degrees will return a
   * Translation2d of {0, 2}.
   */
  Translation2d RotateBy(Rotation2d other) {
    return new Translation2d(
            m_x * other.getCos() - m_y * other.getSin(),
            m_x * other.getSin() + m_y * other.getCos()
    );
  }

  /*
   * Adds two translations in 2d space and returns the sum. This is similar to
   * vector addition.
   *
   * For example, Translation2d{1.0, 2.5} + Translation2d{2.0, 5.5} =
   * Translation2d{3.0, 8.0}
   */
  Translation2d plus(Translation2d other) {
    return new Translation2d(m_x + other.m_x, m_y + other.m_y);
  }

  /**
   * Subtracts the other translation from the other translation and returns the
   * difference.
   * <p>
   * For example, Translation2d{5.0, 4.0} - Translation2d{1.0, 2.0} =
   * Translation2d{4.0, 2.0}
   */
  Translation2d minus(Translation2d other) {
    return new Translation2d(m_x - other.m_x, m_y - other.m_y);
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or simply
   * negating both components of the translation.
   */
  Translation2d unaryMinus() {
    return new Translation2d(-m_x, m_y);
  }

  /**
   * Multiplies the translation by a scalar and returns the new translation.
   * <p>
   * For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}
   */
  Translation2d times(double scalar) {
    return new Translation2d(m_x * scalar, m_y * scalar);
  }

  /**
   * Divides the translation by a scalar and returns the new translation.
   * <p>
   * For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}
   */
  Translation2d div(double scalar) {
    return new Translation2d(m_x / scalar, m_y / scalar);
  }
}
