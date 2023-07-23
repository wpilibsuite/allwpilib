// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"
#include "units/math.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents the state of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  units::meters_per_second_t speed = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModuleState and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const SwerveModuleState& other) const;

  /**
   * Minimize the change in heading the desired swerve module state would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   */
  static SwerveModuleState Optimize(const SwerveModuleState& desiredState,
                                    const Rotation2d& currentAngle);

  /**
   * Adds two SwerveModuleStates and returns the sum. The angle from the current
   * SwerveModuleState is the angle in the final SwerveModuleState.
   *
   * <p>For example, SwerveModuleState{1.0, 180} + SwerveModuleState{2.0, 90} =
   * SwerveModuleState{3.0, 180}
   *
   * @param other The SwerveModuleState to add.
   * @return The sum of the SwerveModuleStates.
   */
  constexpr SwerveModuleState operator+(const SwerveModuleState& other) const {
    return {speed + other.speed, angle};
  }

  /**
   * Subtracts the other SwerveModuleState from the current SwerveModuleState
   * and returns the difference. The angle from the current SwerveModuleState is
   * the angle in the final SwerveModuleState.
   *
   * <p>For example, SwerveModuleState{5.0, 90} - SwerveModuleState{1.0, 42} =
   * SwerveModuleState{4.0, 90}
   *
   * @param other The SwerveModuleState to subtract.
   * @return The difference between the two SwerveModuleStates.
   */
  constexpr SwerveModuleState operator-(const SwerveModuleState& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current SwerveModuleState. This is equivalent to
   * negating all components of the SwerveModuleState. The angle from the
   * current SwerveModuleState is the angle in the final SwerveModuleState.
   *
   * @return The inverse of the current SwerveModuleState.
   */
  constexpr SwerveModuleState operator-() const { return {-speed, angle}; }

  /**
   * Multiplies the SwerveModuleState by a scalar and returns the new
   * SwerveModuleState. The angle from the current SwerveModuleState is the
   * angle in the final SwerveModuleState.
   *
   * <p>For example, SwerveModuleState{2.0, 90} * 2 = SwerveModuleState{4.0, 90}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled SwerveModuleState.
   */
  constexpr SwerveModuleState operator*(double scalar) const {
    return {scalar * speed, angle};
  }

  /**
   * Divides the SwerveModuleState by a scalar and returns the new
   * SwerveModuleState. The angle from the current SwerveModuleState is the
   * angle in the final SwerveModuleState.
   *
   * <p>For example, SwerveModuleState{2.0, 90} / 2 = SwerveModuleState{1.0, 90}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled SwerveModuleState.
   */
  constexpr SwerveModuleState operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }
};
}  // namespace frc
