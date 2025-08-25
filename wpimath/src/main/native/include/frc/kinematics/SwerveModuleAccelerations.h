// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation2d.h"
#include "units/acceleration.h"
#include "units/math.h"

namespace frc {
/**
 * Represents the accelerations of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModuleAccelerations {
  /**
   * Acceleration of the wheel of the module.
   */
  units::meters_per_second_squared_t acceleration = 0_mps_sq;

  /**
   * Angle of the acceleration vector.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModuleAccelerations and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const SwerveModuleAccelerations& other) const {
    return units::math::abs(acceleration - other.acceleration) < 1E-9_mps_sq &&
           angle == other.angle;
  }

  /**
   * Adds two SwerveModuleAccelerations and returns the sum.
   * Note: This performs vector addition of the accelerations.
   *
   * @param other The SwerveModuleAccelerations to add.
   * @return The sum of the SwerveModuleAccelerations.
   */
  SwerveModuleAccelerations operator+(
      const SwerveModuleAccelerations& other) const {
    // Convert to Cartesian coordinates, add, then convert back
    auto thisX = acceleration * angle.Cos();
    auto thisY = acceleration * angle.Sin();
    auto otherX = other.acceleration * other.angle.Cos();
    auto otherY = other.acceleration * other.angle.Sin();

    auto sumX = thisX + otherX;
    auto sumY = thisY + otherY;

    auto resultAcceleration = units::math::hypot(sumX, sumY);
    auto resultAngle = Rotation2d{sumX.value(), sumY.value()};

    return {resultAcceleration, resultAngle};
  }

  /**
   * Subtracts the other SwerveModuleAccelerations from the current
   * SwerveModuleAccelerations and returns the difference.
   *
   * @param other The SwerveModuleAccelerations to subtract.
   * @return The difference between the two SwerveModuleAccelerations.
   */
  SwerveModuleAccelerations operator-(
      const SwerveModuleAccelerations& other) const {
    return *this + (-other);
  }

  /**
   * Returns the inverse of the current SwerveModuleAccelerations.
   * This is equivalent to negating the acceleration magnitude.
   *
   * @return The inverse of the current SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator-() const {
    return {-acceleration, angle + Rotation2d{180_deg}};
  }

  /**
   * Multiplies the SwerveModuleAccelerations by a scalar and returns the new
   * SwerveModuleAccelerations.
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator*(double scalar) const {
    if (scalar < 0) {
      return {-scalar * acceleration, angle + Rotation2d{180_deg}};
    }
    return {scalar * acceleration, angle};
  }

  /**
   * Divides the SwerveModuleAccelerations by a scalar and returns the new
   * SwerveModuleAccelerations.
   *
   * @param scalar The scalar to divide by.
   * @return The scaled SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }
};
}  // namespace frc

#include "frc/kinematics/proto/SwerveModuleAccelerationsProto.h"
#include "frc/kinematics/struct/SwerveModuleAccelerationsStruct.h"
