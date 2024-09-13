// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "units/angle.h"

namespace frc {

/**
 * A rotation in a 2D coordinate frame represented by a point on the unit circle
 * (cosine and sine).
 *
 * The angle is continuous, that is if a Rotation2d is constructed with 361
 * degrees, it will return 361 degrees. This allows algorithms that wouldn't
 * want to see a discontinuity in the rotations as it sweeps past from 360 to 0
 * on the second time around.
 */
class WPILIB_DLLEXPORT Rotation2d {
 public:
  /**
   * Constructs a Rotation2d with a default angle of 0 degrees.
   */
  constexpr Rotation2d() = default;

  /**
   * Constructs a Rotation2d with the given angle.
   *
   * @param value The value of the angle.
   */
  constexpr Rotation2d(units::angle_unit auto value);  // NOLINT

  /**
   * Constructs a Rotation2d with the given x and y (cosine and sine)
   * components. The x and y don't have to be normalized.
   *
   * @param x The x component or cosine of the rotation.
   * @param y The y component or sine of the rotation.
   */
  constexpr Rotation2d(double x, double y);

  /**
   * Adds two rotations together, with the result being bounded between -pi and
   * pi.
   *
   * For example, <code>Rotation2d{30_deg} + Rotation2d{60_deg}</code> equals
   * <code>Rotation2d{units::radian_t{std::numbers::pi/2.0}}</code>
   *
   * @param other The rotation to add.
   *
   * @return The sum of the two rotations.
   */
  constexpr Rotation2d operator+(const Rotation2d& other) const;

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * For example, <code>Rotation2d{10_deg} - Rotation2d{100_deg}</code> equals
   * <code>Rotation2d{units::radian_t{-std::numbers::pi/2.0}}</code>
   *
   * @param other The rotation to subtract.
   *
   * @return The difference between the two rotations.
   */
  constexpr Rotation2d operator-(const Rotation2d& other) const;

  /**
   * Takes the inverse of the current rotation. This is simply the negative of
   * the current angular value.
   *
   * @return The inverse of the current rotation.
   */
  constexpr Rotation2d operator-() const;

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation2d.
   */
  constexpr Rotation2d operator*(double scalar) const;

  /**
   * Divides the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation2d.
   */
  constexpr Rotation2d operator/(double scalar) const;

  /**
   * Checks equality between this Rotation2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Rotation2d& other) const;

  /**
   * Adds the new rotation to the current rotation using a rotation matrix.
   *
   * <pre>
   * [cos_new]   [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   * value_new = std::atan2(sin_new, cos_new)
   * </pre>
   *
   * @param other The rotation to rotate by.
   *
   * @return The new rotated Rotation2d.
   */
  constexpr Rotation2d RotateBy(const Rotation2d& other) const;

  /**
   * Returns the radian value of the rotation.
   *
   * @return The radian value of the rotation.
   * @see AngleModulus to constrain the angle within (-pi, pi]
   */
  constexpr units::radian_t Radians() const { return m_value; }

  /**
   * Returns the degree value of the rotation.
   *
   * @return The degree value of the rotation.
   * @see InputModulus to constrain the angle within (-180, 180]
   */
  constexpr units::degree_t Degrees() const { return m_value; }

  /**
   * Returns the cosine of the rotation.
   *
   * @return The cosine of the rotation.
   */
  constexpr double Cos() const { return m_cos; }

  /**
   * Returns the sine of the rotation.
   *
   * @return The sine of the rotation.
   */
  constexpr double Sin() const { return m_sin; }

  /**
   * Returns the tangent of the rotation.
   *
   * @return The tangent of the rotation.
   */
  constexpr double Tan() const { return Sin() / Cos(); }

 private:
  units::radian_t m_value = 0_rad;
  double m_cos = 1;
  double m_sin = 0;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Rotation2d& rotation);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Rotation2d& rotation);

}  // namespace frc

#include "frc/geometry/proto/Rotation2dProto.h"
#include "frc/geometry/struct/Rotation2dStruct.h"
#include "frc/geometry/Rotation2d.inc"
