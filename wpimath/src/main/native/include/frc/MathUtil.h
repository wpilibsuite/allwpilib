// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <wpi/SymbolExports.h>

#include "units/angle.h"
#include "units/base.h"
#include "units/math.h"

namespace frc {

/**
 * Returns 0.0 if the given value is within the specified range around zero. The
 * remaining range between the deadband and the maximum magnitude is scaled from
 * 0.0 to the maximum magnitude.
 *
 * @param value Value to clip.
 * @param deadband Range around zero.
 * @param maxMagnitude The maximum magnitude of the input (defaults to 1). Can
 * be infinite.
 * @return The value after the deadband is applied.
 */
template <typename T,
          typename = std::enable_if_t<std::disjunction_v<
              std::is_floating_point<T>, units::traits::is_unit_t<T>>>>
T ApplyDeadband(T value, T deadband, T maxMagnitude = T{1.0}) {
  T magnitude;
  if constexpr (std::is_floating_point_v<T>) {
    magnitude = std::abs(value);
  } else {
    magnitude = units::math::abs(value);
  }

  if (magnitude > deadband) {
    if (maxMagnitude / deadband > 1.0E12) {
      // If max magnitude is sufficiently large, the implementation encounters
      // roundoff error.  Implementing the limiting behavior directly avoids
      // the problem.
      return value > T{0.0} ? value - deadband : value + deadband;
    }
    if (value > T{0.0}) {
      // Map deadband to 0 and map max to max.
      //
      // y - y₁ = m(x - x₁)
      // y - y₁ = (y₂ - y₁)/(x₂ - x₁) (x - x₁)
      // y = (y₂ - y₁)/(x₂ - x₁) (x - x₁) + y₁
      //
      // (x₁, y₁) = (deadband, 0) and (x₂, y₂) = (max, max).
      // x₁ = deadband
      // y₁ = 0
      // x₂ = max
      // y₂ = max
      //
      // y = (max - 0)/(max - deadband) (x - deadband) + 0
      // y = max/(max - deadband) (x - deadband)
      // y = max (x - deadband)/(max - deadband)
      return maxMagnitude * (value - deadband) / (maxMagnitude - deadband);
    } else {
      // Map -deadband to 0 and map -max to -max.
      //
      // y - y₁ = m(x - x₁)
      // y - y₁ = (y₂ - y₁)/(x₂ - x₁) (x - x₁)
      // y = (y₂ - y₁)/(x₂ - x₁) (x - x₁) + y₁
      //
      // (x₁, y₁) = (-deadband, 0) and (x₂, y₂) = (-max, -max).
      // x₁ = -deadband
      // y₁ = 0
      // x₂ = -max
      // y₂ = -max
      //
      // y = (-max - 0)/(-max + deadband) (x + deadband) + 0
      // y = max/(max - deadband) (x + deadband)
      // y = max (x + deadband)/(max - deadband)
      return maxMagnitude * (value + deadband) / (maxMagnitude - deadband);
    }
  } else {
    return T{0.0};
  }
}

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
                                       units::radian_t{-std::numbers::pi},
                                       units::radian_t{std::numbers::pi});
}

}  // namespace frc
