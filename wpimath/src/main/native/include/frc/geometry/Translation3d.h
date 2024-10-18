// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Rotation3d.h"
#include "frc/geometry/Translation2d.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {

/**
 * Represents a translation in 3D space.
 * This object can be used to represent a point or a vector.
 *
 * This assumes that you are using conventional mathematical axes. When the
 * robot is at the origin facing in the positive X direction, forward is
 * positive X, left is positive Y, and up is positive Z.
 */
class WPILIB_DLLEXPORT Translation3d {
 public:
  /**
   * Constructs a Translation3d with X, Y, and Z components equal to zero.
   */
  constexpr Translation3d() = default;

  /**
   * Constructs a Translation3d with the X, Y, and Z components equal to the
   * provided values.
   *
   * @param x The x component of the translation.
   * @param y The y component of the translation.
   * @param z The z component of the translation.
   */
  constexpr Translation3d(units::meter_t x, units::meter_t y, units::meter_t z)
      : m_x{x}, m_y{y}, m_z{z} {}

  /**
   * Constructs a Translation3d with the provided distance and angle. This is
   * essentially converting from polar coordinates to Cartesian coordinates.
   *
   * @param distance The distance from the origin to the end of the translation.
   * @param angle The angle between the x-axis and the translation vector.
   */
  constexpr Translation3d(units::meter_t distance, const Rotation3d& angle) {
    auto rectangular = Translation3d{distance, 0_m, 0_m}.RotateBy(angle);
    m_x = rectangular.X();
    m_y = rectangular.Y();
    m_z = rectangular.Z();
  }

  /**
   * Constructs a Translation3d from the provided translation vector's X, Y, and
   * Z components. The values are assumed to be in meters.
   *
   * @param vector The translation vector to represent.
   */
  constexpr explicit Translation3d(const Eigen::Vector3d& vector)
      : m_x{units::meter_t{vector.x()}},
        m_y{units::meter_t{vector.y()}},
        m_z{units::meter_t{vector.z()}} {}

  /**
   * Calculates the distance between two translations in 3D space.
   *
   * The distance between translations is defined as
   * √((x₂−x₁)²+(y₂−y₁)²+(z₂−z₁)²).
   *
   * @param other The translation to compute the distance to.
   *
   * @return The distance between the two translations.
   */
  constexpr units::meter_t Distance(const Translation3d& other) const {
    return units::math::sqrt(units::math::pow<2>(other.m_x - m_x) +
                             units::math::pow<2>(other.m_y - m_y) +
                             units::math::pow<2>(other.m_z - m_z));
  }

  /**
   * Returns the X component of the translation.
   *
   * @return The Z component of the translation.
   */
  constexpr units::meter_t X() const { return m_x; }

  /**
   * Returns the Y component of the translation.
   *
   * @return The Y component of the translation.
   */
  constexpr units::meter_t Y() const { return m_y; }

  /**
   * Returns the Z component of the translation.
   *
   * @return The Z component of the translation.
   */
  constexpr units::meter_t Z() const { return m_z; }

  /**
   * Returns a vector representation of this translation.
   *
   * @return A Vector representation of this translation.
   */
  constexpr Eigen::Vector3d ToVector() const {
    return Eigen::Vector3d{{m_x.value(), m_y.value(), m_z.value()}};
  }

  /**
   * Returns the norm, or distance from the origin to the translation.
   *
   * @return The norm of the translation.
   */
  constexpr units::meter_t Norm() const {
    return units::math::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
  }

  /**
   * Applies a rotation to the translation in 3D space.
   *
   * For example, rotating a Translation3d of &lt;2, 0, 0&gt; by 90 degrees
   * around the Z axis will return a Translation3d of &lt;0, 2, 0&gt;.
   *
   * @param other The rotation to rotate the translation by.
   *
   * @return The new rotated translation.
   */
  constexpr Translation3d RotateBy(const Rotation3d& other) const {
    Quaternion p{0.0, m_x.value(), m_y.value(), m_z.value()};
    auto qprime = other.GetQuaternion() * p * other.GetQuaternion().Inverse();
    return Translation3d{units::meter_t{qprime.X()}, units::meter_t{qprime.Y()},
                         units::meter_t{qprime.Z()}};
  }

  /**
   * Returns a Translation2d representing this Translation3d projected into the
   * X-Y plane.
   */
  constexpr Translation2d ToTranslation2d() const {
    return Translation2d{m_x, m_y};
  }

  /**
   * Returns the sum of two translations in 3D space.
   *
   * For example, Translation3d{1.0, 2.5, 3.5} + Translation3d{2.0, 5.5, 7.5} =
   * Translation3d{3.0, 8.0, 11.0}.
   *
   * @param other The translation to add.
   *
   * @return The sum of the translations.
   */
  constexpr Translation3d operator+(const Translation3d& other) const {
    return {X() + other.X(), Y() + other.Y(), Z() + other.Z()};
  }

  /**
   * Returns the difference between two translations.
   *
   * For example, Translation3d{5.0, 4.0, 3.0} - Translation3d{1.0, 2.0, 3.0} =
   * Translation3d{4.0, 2.0, 0.0}.
   *
   * @param other The translation to subtract.
   *
   * @return The difference between the two translations.
   */
  constexpr Translation3d operator-(const Translation3d& other) const {
    return operator+(-other);
  }

  /**
   * Returns the inverse of the current translation. This is equivalent to
   * negating all components of the translation.
   *
   * @return The inverse of the current translation.
   */
  constexpr Translation3d operator-() const { return {-m_x, -m_y, -m_z}; }

  /**
   * Returns the translation multiplied by a scalar.
   *
   * For example, Translation3d{2.0, 2.5, 4.5} * 2 = Translation3d{4.0, 5.0,
   * 9.0}.
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled translation.
   */
  constexpr Translation3d operator*(double scalar) const {
    return {scalar * m_x, scalar * m_y, scalar * m_z};
  }

  /**
   * Returns the translation divided by a scalar.
   *
   * For example, Translation3d{2.0, 2.5, 4.5} / 2 = Translation3d{1.0, 1.25,
   * 2.25}.
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled translation.
   */
  constexpr Translation3d operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Checks equality between this Translation3d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Translation3d& other) const {
    return units::math::abs(m_x - other.m_x) < 1E-9_m &&
           units::math::abs(m_y - other.m_y) < 1E-9_m &&
           units::math::abs(m_z - other.m_z) < 1E-9_m;
  }

 private:
  units::meter_t m_x = 0_m;
  units::meter_t m_y = 0_m;
  units::meter_t m_z = 0_m;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Translation3d& state);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Translation3d& state);

}  // namespace frc

#ifndef NO_PROTOBUF
#include "frc/geometry/proto/Translation3dProto.h"
#endif
#include "frc/geometry/struct/Translation3dStruct.h"
