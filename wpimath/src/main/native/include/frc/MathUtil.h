// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>
#include <wpi/numbers>

#include "units/angle.h"

namespace frc {

/**
 * Returns 0.0 if the given value is within the specified range around zero.
 * The remaining range between the deadband and 1.0 is scaled from 0.0 to 1.0.
 *
 * @param value    Value to clip.
 * @param deadband Range around zero.
 */
WPILIB_DLLEXPORT
double ApplyDeadband(double value, double deadband);

/**
 * Returns modulus of input.
 *
 * @param input        Input value to wrap.
 * @param minimumInput The minimum value expected from the input.
 * @param maximumInput The maximum value expected from the input.
 */
template <typename T>
constexpr T InputModulus(T input, T minimumInput, T maximumInput) {
  T modulus = maximumInput - minimumInput;

  // Wrap input if it's above the maximum input
  int numMax = (input - minimumInput) / modulus;
  input -= numMax * modulus;

  // Wrap input if it's below the minimum input
  int numMin = (input - maximumInput) / modulus;
  input -= numMin * modulus;

  return input;
}

/**
 * Wraps an angle to the range -pi to pi radians (-180 to 180 degrees).
 *
 * @param angle Angle to wrap.
 */
WPILIB_DLLEXPORT
constexpr units::radian_t AngleModulus(units::radian_t angle) {
  return InputModulus<units::radian_t>(angle,
                                       units::radian_t{-wpi::numbers::pi},
                                       units::radian_t{wpi::numbers::pi});
}

}  // namespace frc
