// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <initializer_list>
#include <span>

#include <Eigen/Core>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/area.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/json_fwd.hpp"

namespace wpi::math {

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
  constexpr Translation2d(wpi::units::meter_t x, wpi::units::meter_t y)
      : m_x{x}, m_y{y} {}

  /**
   * Constructs a Translation2d with the provided distance and angle. This is
   * essentially converting from polar coordinates to Cartesian coordinates.
   *
   * @param distance The distance from the origin to the end of the translation.
   * @param angle The angle between the x-axis and the translation vector.
   */
  constexpr Translation2d(wpi::units::meter_t distance, const Rotation2d& angle)
      : m_x{distance * angle.Cos()}, m_y{distance * angle.Sin()} {}

  /**
   * Constructs a Translation2d from a 2D translation vector. The values are
   * assumed to be in meters.
   *
   * @param vector The translation vector.
   */
  constexpr explicit Translation2d(const Eigen::Vector2d& vector)
      : m_x{wpi::units::meter_t{vector.x()}},
        m_y{wpi::units::meter_t{vector.y()}} {}

  /**
   * Calculates the distance between two translations in 2D space.
   *
   * The distance between translations is defined as √((x₂−x₁)²+(y₂−y₁)²).
   *
   * @param other The translation to compute the distance to.
   *
   * @return The distance between the two translations.
   */
  constexpr wpi::units::meter_t Distance(const Translation2d& other) const {
    return wpi::units::math::hypot(other.m_x - m_x, other.m_y - m_y);
  }

  /**
   * Calculates the square of the distance between two translations in 2D space.
   * This is equivalent to squaring the result of Distance(Translation2d), but
   * avoids computing a square root.
   *
   * The square of the distance between translations is defined as
   * (x₂−x₁)²+(y₂−y₁)².
   *
   * @param other The translation to compute the squared distance to.
   * @return The square of the distance between the two translations.
   */
  constexpr wpi::units::square_meter_t SquaredDistance(
      const Translation2d& other) const {
    return wpi::units::math::pow<2>(other.m_x - m_x) +
           wpi::units::math::pow<2>(other.m_y - m_y);
  }

  /**
   * Returns the X component of the translation.
   *
   * @return The X component of the translation.
   */
  constexpr wpi::units::meter_t X() const { return m_x; }

  /**
   * Returns the Y component of the translation.
   *
   * @return The Y component of the translation.
   */
  constexpr wpi::units::meter_t Y() const { return m_y; }

  /**
   * Returns a 2D translation vector representation of this translation.
   *
   * @return A 2D translation vector representation of this translation.
   */
  constexpr Eigen::Vector2d ToVector() const {
    return Eigen::Vector2d{{m_x.value(), m_y.value()}};
  }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  constexpr wpi::units::meter_t Norm() const {
    return wpi::units::math::hypot(m_x, m_y);
  }

  /**
   * Returns the squared norm, or squared distance from the origin to the
   * translation. This is equivalent to squaring the result of Norm(), but
   * avoids computing a square root.
   *
   * @return The squared norm of the translation.
   */
  constexpr wpi::units::square_meter_t SquaredNorm() const {
    return wpi::units::math::pow<2>(m_x) + wpi::units::math::pow<2>(m_y);
  }

  /**
   * Returns the angle this translation forms with the positive X axis.
   *
   * @return The angle of the translation
   */
  constexpr Rotation2d Angle() const {
    return Rotation2d{m_x.value(), m_y.value()};
  }

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
  constexpr Translation2d RotateBy(const Rotation2d& other) const {
    return {m_x * other.Cos() - m_y * other.Sin(),
            m_x * other.Sin() + m_y * other.Cos()};
  }

  /**
   * Rotates this translation around another translation in 2D space.
   *
   * <pre>
   * [x_new]   [rot.cos, -rot.sin][x - other.x]   [other.x]
   * [y_new] = [rot.sin,  rot.cos][y - other.y] + [other.y]
   * </pre>
   *
   * @param other The other translation to rotate around.
   * @param rot The rotation to rotate the translation by.
   * @return The new rotated translation.
   */
  constexpr Translation2d RotateAround(const Translation2d& other,
                                       const Rotation2d& rot) const {
    return {(m_x - other.X()) * rot.Cos() - (m_y - other.Y()) * rot.Sin() +
                other.X(),
            (m_x - other.X()) * rot.Sin() + (m_y - other.Y()) * rot.Cos() +
                other.Y()};
  }

  /**
   * Computes the dot product between this translation and another translation
   * in 2D space.
   *
   * The dot product between two translations is defined as x₁x₂+y₁y₂.
   *
   * @param other The translation to compute the dot product with.
   * @return The dot product between the two translations.
   */
  constexpr wpi::units::square_meter_t Dot(const Translation2d& other) const {
    return m_x * other.X() + m_y * other.Y();
  }

  /**
   * Computes the cross product between this translation and another translation
   * in 2D space.
   *
   * The 2D cross product between two translations is defined as x₁y₂-x₂y₁.
   *
   * @param other The translation to compute the cross product with.
   * @return The cross product between the two translations.
   */
  constexpr wpi::units::square_meter_t Cross(const Translation2d& other) const {
    return m_x * other.Y() - m_y * other.X();
  }

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
  constexpr Translation2d operator+(const Translation2d& other) const {
    return {X() + other.X(), Y() + other.Y()};
  }

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
  constexpr Translation2d operator-(const Translation2d& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * rotating by 180 degrees, flipping the point over both axes, or negating all
   * components of the translation.
   *
   * @return The inverse of the current translation.
   */
  constexpr Translation2d operator-() const { return {-m_x, -m_y}; }

  /**
   * Returns the translation multiplied by a scalar.
   *
   * For example, Translation2d{2.0, 2.5} * 2 = Translation2d{4.0, 5.0}.
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled translation.
   */
  constexpr Translation2d operator*(double scalar) const {
    return {scalar * m_x, scalar * m_y};
  }

  /**
   * Returns the translation divided by a scalar.
   *
   * For example, Translation2d{2.0, 2.5} / 2 = Translation2d{1.0, 1.25}.
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled translation.
   */
  constexpr Translation2d operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Checks equality between this Translation2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Translation2d& other) const {
    return wpi::units::math::abs(m_x - other.m_x) < 1E-9_m &&
           wpi::units::math::abs(m_y - other.m_y) < 1E-9_m;
  }

  /**
   * Returns the nearest Translation2d from a collection of translations
   * @param translations The collection of translations.
   * @return The nearest Translation2d from the collection.
   */
  constexpr Translation2d Nearest(
      std::span<const Translation2d> translations) const {
    return *std::min_element(
        translations.begin(), translations.end(),
        [this](const Translation2d& a, const Translation2d& b) {
          return this->Distance(a) < this->Distance(b);
        });
  }

  /**
   * Returns the nearest Translation2d from a collection of translations
   * @param translations The collection of translations.
   * @return The nearest Translation2d from the collection.
   */
  constexpr Translation2d Nearest(
      std::initializer_list<Translation2d> translations) const {
    return *std::min_element(
        translations.begin(), translations.end(),
        [this](const Translation2d& a, const Translation2d& b) {
          return this->Distance(a) < this->Distance(b);
        });
  }

 private:
  wpi::units::meter_t m_x = 0_m;
  wpi::units::meter_t m_y = 0_m;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const Translation2d& state);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, Translation2d& state);

}  // namespace wpi::math

#include "wpi/math/geometry/proto/Translation2dProto.hpp"
#include "wpi/math/geometry/struct/Translation2dStruct.hpp"
