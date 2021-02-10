// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Rotation2d.h"
#include "units/length.h"

namespace wpi {
class json;
}  // namespace wpi

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
  constexpr Translation2d() = default;

  /**
   * Constructs a Translation2d with the X and Y components equal to the
   * provided values.
   *
   * @param x The x component of the translation.
   * @param y The y component of the translation.
   */
  Translation2d(units::meter_t x, units::meter_t y);

  /**
   * Constructs a Translation2d with the provided distance and angle. This is
   * essentially converting from polar coordinates to Cartesian coordinates.
   *
   * @param distance The distance from the origin to the end of the translation.
   * @param angle The angle between the x-axis and the translation vector.
   */
  Translation2d(units::meter_t distance, const Rotation2d& angle);

  /**
   * Calculates the distance between two translations in 2d space.
   *
   * This function uses the pythagorean theorem to calculate the distance.
   * distance = std::sqrt((x2 - x1)^2 + (y2 - y1)^2)
   *
   * @param other The translation to compute the distance to.
   *
   * @return The distance between the two translations.
   */
  units::meter_t Distance(const Translation2d& other) const;

  /**
   * Returns the X component of the translation.
   *
   * @return The x component of the translation.
   */
  units::meter_t X() const { return m_x; }

  /**
   * Returns the Y component of the translation.
   *
   * @return The y component of the translation.
   */
  units::meter_t Y() const { return m_y; }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  units::meter_t Norm() const;

  /**
   * Applies a rotation to the translation in 2d space.
   *
   * This multiplies the translation vector by a counterclockwise rotation
   * matrix of the given angle.
   *
   * [x_new]   [other.cos, -other.sin][x]
   * [y_new] = [other.sin,  other.cos][y]
   *
   * For example, rotating a Translation2d of {2, 0} by 90 degrees will return a
   * Translation2d of {0, 2}.
   *
   * @param other The rotation to rotate the translation by.
   *
   * @return The new rotated translation.
   */
  Translation2d RotateBy(const Rotation2d& other) const;

  /**
   * Adds two translations in 2d space and returns the sum. This is similar to
   * vector addition.
   *
   * For example, Translation2d{1.0, 2.5} + Translation2d{2.0, 5.5} =
   * Translation2d{3.0, 8.0}
   *
   * @param other The translation to add.
   *
   * @return The sum of the translations.
   */
  Translation2d operator+(const Translation2d& other) const;

  /**
   * Adds the new translation to the current translation.
   *
   * This is similar to the + operator, except that the current object is
   * mutated.
   *
   * @param other The translation to add.
   *
   * @return The reference to the new mutated object.
   */
  Translation2d& operator+=(const Translation2d& other);

  /**
   * Subtracts the other translation from the other translation and returns the
   * difference.
   *
   * For example, Translation2d{5.0, 4.0} - Translation2d{1.0, 2.0} =
   * Translation2d{4.0, 2.0}
   *
   * @param other The translation to subtract.
   *
   * @return The difference between the two translations.
   */
  Translation2d operator-(const Translation2d& other) const;

  /**
   * Subtracts the new translation from the current translation.
   *
   * This is similar to the - operator, except that the current object is
   * mutated.
   *
   * @param other The translation to subtract.
   *
   * @return The reference to the new mutated object.
   */
  Translation2d& operator-=(const Translation2d& other);

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or simply
   * negating both components of the translation.
   *
   * @return The inverse of the current translation.
   */
  Translation2d operator-() const;

  /**
   * Multiplies the translation by a scalar and returns the new translation.
   *
   * For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled translation.
   */
  Translation2d operator*(double scalar) const;

  /**
   * Multiplies the current translation by a scalar.
   *
   * This is similar to the * operator, except that current object is mutated.
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The reference to the new mutated object.
   */
  Translation2d& operator*=(double scalar);

  /**
   * Divides the translation by a scalar and returns the new translation.
   *
   * For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled translation.
   */
  Translation2d operator/(double scalar) const;

  /**
   * Checks equality between this Translation2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Translation2d& other) const;

  /**
   * Checks inequality between this Translation2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are not equal.
   */
  bool operator!=(const Translation2d& other) const;

  /*
   * Divides the current translation by a scalar.
   *
   * This is similar to the / operator, except that current object is mutated.
   *
   * @param scalar The scalar to divide by.
   *
   * @return The reference to the new mutated object.
   */
  Translation2d& operator/=(double scalar);

 private:
  units::meter_t m_x = 0_m;
  units::meter_t m_y = 0_m;
};

void to_json(wpi::json& json, const Translation2d& state);

void from_json(const wpi::json& json, Translation2d& state);

}  // namespace frc
