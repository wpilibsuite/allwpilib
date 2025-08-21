// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "units/acceleration.h"
#include "units/angular_acceleration.h"
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
   * Angular acceleration of the module.
   */
  units::radians_per_second_squared_t angularAcceleration = 0_rad_per_s_sq;

  /**
   * Checks equality between this SwerveModuleAccelerations and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const SwerveModuleAccelerations& other) const {
    return units::math::abs(acceleration - other.acceleration) < 1E-9_mps_sq &&
           units::math::abs(angularAcceleration - other.angularAcceleration) < 1E-9_rad_per_s_sq;
  }

  /**
   * Adds two SwerveModuleAccelerations and returns the sum.
   *
   * @param other The SwerveModuleAccelerations to add.
   * @return The sum of the SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator+(
      const SwerveModuleAccelerations& other) const {
    return {acceleration + other.acceleration, angularAcceleration + other.angularAcceleration};
  }

  /**
   * Subtracts the other SwerveModuleAccelerations from the current
   * SwerveModuleAccelerations and returns the difference.
   *
   * @param other The SwerveModuleAccelerations to subtract.
   * @return The difference between the two SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator-(
      const SwerveModuleAccelerations& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current SwerveModuleAccelerations.
   * This is equivalent to negating all components of the
   * SwerveModuleAccelerations.
   *
   * @return The inverse of the current SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator-() const {
    return {-acceleration, -angularAcceleration};
  }

  /**
   * Multiplies the SwerveModuleAccelerations by a scalar and returns the new
   * SwerveModuleAccelerations.
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled SwerveModuleAccelerations.
   */
  constexpr SwerveModuleAccelerations operator*(double scalar) const {
    return {scalar * acceleration, scalar * angularAcceleration};
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
