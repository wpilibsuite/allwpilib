// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>

#include <Eigen/Core>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Rotation2d.h"
#include "units/length.h"

namespace frc {

/**
 * Represents a translation in 2D space.
 * This object can be used to represent a point or a vector.
 *
 * This assumes that you are using conventional mathematical axes.
 * When the robot is at the origin facing in the positive X direction, forward
 * is positive X and left is positive Y.
 */
class WPILIB_DLLEXPORT Translation2d {
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
  constexpr Translation2d(units::meter_t x, units::meter_t y);

  /**
   * Constructs a Translation2d with the provided distance and angle. This is
   * essentially converting from polar coordinates to Cartesian coordinates.
   *
   * @param distance The distance from the origin to the end of the translation.
   * @param angle The angle between the x-axis and the translation vector.
   */
  constexpr Translation2d(units::meter_t distance, const Rotation2d& angle);

  /**
   * Constructs a Translation2d from the provided translation vector's X and Y
   * components. The values are assumed to be in meters.
   *
   * @param vector The translation vector to represent.
   */
  explicit Translation2d(const Eigen::Vector2d& vector);

  /**
   * Calculates the distance between two translations in 2D space.
   *
   * The distance between translations is defined as √((x₂−x₁)²+(y₂−y₁)²).
   *
   * @param other The translation to compute the distance to.
   *
   * @return The distance between the two translations.
   */
  units::meter_t Distance(const Translation2d& other) const;

  /**
   * Returns the X component of the translation.
   *
   * @return The X component of the translation.
   */
  constexpr units::meter_t X() const { return m_x; }

  /**
   * Returns the Y component of the translation.
   *
   * @return The Y component of the translation.
   */
  constexpr units::meter_t Y() const { return m_y; }

  /**
   * Returns a vector representation of this translation.
   *
   * @return A Vector representation of this translation.
   */
  constexpr Eigen::Vector2d ToVector() const;

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  units::meter_t Norm() const;

  /**
   * Returns the angle this translation forms with the positive X axis.
   *
   * @return The angle of the translation
   */
  constexpr Rotation2d Angle() const;

  /**
   * Applies a rotation to the translation in 2D space.
   *
   * This multiplies the translation vector by a counterclockwise rotation
   * matrix of the given angle.
   *
   * <pre>
   * [x_new]   [other.cos, -other.sin][x]
   * [y_new] = [other.sin,  other.cos][y]
   * </pre>
   *
   * For example, rotating a Translation2d of &lt;2, 0&gt; by 90 degrees will
   * return a Translation2d of &lt;0, 2&gt;.
   *
   * @param other The rotation to rotate the translation by.
   *
   * @return The new rotated translation.
   */
  constexpr Translation2d RotateBy(const Rotation2d& other) const;

  /**
   * Returns the sum of two translations in 2D space.
   *
   * For example, Translation3d{1.0, 2.5} + Translation3d{2.0, 5.5} =
   * Translation3d{3.0, 8.0}.
   *
   * @param other The translation to add.
   *
   * @return The sum of the translations.
   */
  constexpr Translation2d operator+(const Translation2d& other) const;

  /**
   * Returns the difference between two translations.
   *
   * For example, Translation2d{5.0, 4.0} - Translation2d{1.0, 2.0} =
   * Translation2d{4.0, 2.0}.
   *
   * @param other The translation to subtract.
   *
   * @return The difference between the two translations.
   */
  constexpr Translation2d operator-(const Translation2d& other) const;

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or negating all
   * components of the translation.
   *
   * @return The inverse of the current translation.
   */
  constexpr Translation2d operator-() const;

  /**
   * Returns the translation multiplied by a scalar.
   *
   * For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}.
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled translation.
   */
  constexpr Translation2d operator*(double scalar) const;

  /**
   * Returns the translation divided by a scalar.
   *
   * For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}.
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled translation.
   */
  constexpr Translation2d operator/(double scalar) const;

  /**
   * Checks equality between this Translation2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Translation2d& other) const;

  /**
   * Returns the nearest Translation2d from a collection of translations
   * @param translations The collection of translations.
   * @return The nearest Translation2d from the collection.
   */
  Translation2d Nearest(std::span<const Translation2d> translations) const;

  /**
   * Returns the nearest Translation2d from a collection of translations
   * @param translations The collection of translations.
   * @return The nearest Translation2d from the collection.
   */
  Translation2d Nearest(
      std::initializer_list<Translation2d> translations) const;

 private:
  units::meter_t m_x = 0_m;
  units::meter_t m_y = 0_m;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Translation2d& state);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Translation2d& state);

}  // namespace frc

#include "frc/geometry/proto/Translation2dProto.h"
#include "frc/geometry/struct/Translation2dStruct.h"
#include "frc/geometry/Translation2d.inc"
