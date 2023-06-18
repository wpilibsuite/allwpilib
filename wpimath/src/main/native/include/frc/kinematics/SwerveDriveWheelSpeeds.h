// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/kinematics/SwerveModuleState.h"

namespace frc {
/**
 * Represents the wheel positions for a swerve drive drivetrain.
 */
template <size_t NumModules>
struct WPILIB_DLLEXPORT SwerveDriveWheelStates {
  /**
   * The states (angle and speed) of the modules.
   */
  wpi::array<SwerveModuleState, NumModules> states;

  /**
   * Checks equality between this SwerveDriveWheelStates and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const SwerveDriveWheelStates& other) const = default;

  /**
   * Checks inequality between this SwerveDriveWheelStates and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are not equal.
   */
  bool operator!=(const SwerveDriveWheelStates& other) const = default;

  SwerveDriveWheelStates<NumModules> operator-(
      const SwerveDriveWheelStates<NumModules>& other) const {
    auto result = wpi::array<SwerveModuleState, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      result[i] = positions[i] - other.positions[i];
    }
    return {result};
  }

  SwerveDriveWheelStates<NumModules> Interpolate(
      const SwerveDriveWheelStates<NumModules>& endValue, double t) const {
    auto result = wpi::array<SwerveModuleState, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      result[i] = positions[i].Interpolate(endValue.positions[i], t);
    }
    return {result};
  }
};
}  // namespace frc
