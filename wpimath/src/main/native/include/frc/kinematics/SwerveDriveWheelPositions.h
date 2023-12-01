// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/kinematics/SwerveModulePosition.h"

namespace frc {
/**
 * Represents the wheel positions for a swerve drive drivetrain.
 */
template <size_t NumModules>
struct WPILIB_DLLEXPORT SwerveDriveWheelPositions {
  /**
   * The distances driven by the wheels.
   */
  wpi::array<SwerveModulePosition, NumModules> positions;

  /**
   * Checks equality between this SwerveDriveWheelPositions and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const SwerveDriveWheelPositions& other) const = default;

  /**
   * Checks inequality between this SwerveDriveWheelPositions and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are not equal.
   */
  bool operator!=(const SwerveDriveWheelPositions& other) const = default;

  SwerveDriveWheelPositions<NumModules> Interpolate(
      const SwerveDriveWheelPositions<NumModules>& endValue, double t) const {
    auto result =
        wpi::array<SwerveModulePosition, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      result[i] = positions[i].Interpolate(endValue.positions[i], t);
    }
    return {result};
  }
};
}  // namespace frc
