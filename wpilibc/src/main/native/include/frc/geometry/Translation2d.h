/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Rotation2d.h"

namespace frc {

/**
 * Represents a translation in 2d space.
 * This object can be used to represent a point or a vector.
 *
 * This assumes that you are using conventional mathematical axes.
 * When the robot is placed on the origin, facing toward the X direction,
 * moving forward increases the X, whereas moving to the left increases the Y.
 */
class Translation2d {
 public:
  /**
   * Constructs a Translation2d with X and Y components equal to zero.
   */
  Translation2d();

  /**
   * Constructs a Translation2d with the X and Y components equal to the
   * provided values.
   */
  Translation2d(double x, double y);

  /**
   * Calculates the distance between two translations in 2d space.
   *
   * This function uses the pythagorean theorem to calculate the distance.
   * distance = sqrt((x2 - x1)^2 + (y2 - y1)^2)
   */
  double Distance(const Translation2d& other) const;

  /*
   * Returns the X component of the translation.
   */
  double X() const { return m_x; }

  /*
   * Returns the Y component of the translation.
   */
  double Y() const { return m_y; }

  /*
   * Returns the norm, or distance from the origin to the translation.
   */
  double Norm() const;

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
  Translation2d RotateBy(const Rotation2d& other) const;

  /*
   * Adds two translations in 2d space and returns the sum. This is similar to
   * vector addition.
   *
   * For example, Translation2d{1.0, 2.5} + Translation2d{2.0, 5.5} =
   * Translation2d{3.0, 8.0}
   */
  Translation2d operator+(const Translation2d& other) const;

  /**
   * Adds the new translation to the current translation.
   *
   * This is similar to the + operator, except that the current object is
   * mutated.
   */
  Translation2d& operator+=(const Translation2d& other);

  /**
   * Subtracts the other translation from the other translation and returns the
   * difference.
   *
   * For example, Translation2d{5.0, 4.0} - Translation2d{1.0, 2.0} =
   * Translation2d{4.0, 2.0}
   */
  Translation2d operator-(const Translation2d& other) const;

  /**
   * Subtracts the new translation from the current translation.
   *
   * This is similar to the - operator, except that the current object is
   * mutated.
   */
  Translation2d& operator-=(const Translation2d& other);

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or simply
   * negating both components of the translation.
   */
  Translation2d operator-() const;

  /**
   * Multiplies the translation by a scalar and returns the new translation.
   *
   * For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}
   */
  Translation2d operator*(double scalar) const;

  /*
   * Multiplies the current translation by a scalar.
   *
   * This is similar to the * operator, except that current object is mutated.
   */
  Translation2d& operator*=(double scalar);

  /**
   * Divides the translation by a scalar and returns the new translation.
   *
   * For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}
   */
  Translation2d operator/(double scalar) const;

  /*
   * Divides the current translation by a scalar.
   *
   * This is similar to the / operator, except that current object is mutated.
   */
  Translation2d& operator/=(double scalar);

 private:
  double m_x;
  double m_y;
};
}  // namespace frc
