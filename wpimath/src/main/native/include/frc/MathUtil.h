// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>
#include <type_traits>

#include <gcem.hpp>
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
template <typename T>
  requires std::is_arithmetic_v<T> || units::traits::is_unit_t_v<T>
constexpr T ApplyDeadband(T value, T deadband, T maxMagnitude = T{1.0}) {
  T magnitude;
  if constexpr (std::is_arithmetic_v<T>) {
    magnitude = gcem::abs(value);
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
 * Checks if the given value matches an expected value within a certain
 * tolerance.
 *
 * @param expected The expected value
 * @param actual The actual value
 * @param tolerance The allowed difference between the actual and the expected
 * value
 * @return Whether or not the actual value is within the allowed tolerance
 */
template <typename T>
  requires std::is_arithmetic_v<T> || units::traits::is_unit_t_v<T>
constexpr bool IsNear(T expected, T actual, T tolerance) {
  if constexpr (std::is_arithmetic_v<T>) {
    return std::abs(expected - actual) < tolerance;
  } else {
    return units::math::abs(expected - actual) < tolerance;
  }
}

/**
 * Checks if the given value matches an expected value within a certain
 * tolerance. Supports continuous input for cases like absolute encoders.
 *
 * Continuous input means that the min and max value are considered to be the
 * same point, and tolerances can be checked across them. A common example
 * would be for absolute encoders: calling isNear(2, 359, 5, 0, 360) returns
 * true because 359 is 1 away from 360 (which is treated as the same as 0) and
 * 2 is 2 away from 0, adding up to an error of 3 degrees, which is within the
 * given tolerance of 5.
 *
 * @param expected The expected value
 * @param actual The actual value
 * @param tolerance The allowed difference between the actual and the expected
 * value
 * @param min Smallest value before wrapping around to the largest value
 * @param max Largest value before wrapping around to the smallest value
 * @return Whether or not the actual value is within the allowed tolerance
 */
template <typename T>
  requires std::is_arithmetic_v<T> || units::traits::is_unit_t_v<T>
constexpr bool IsNear(T expected, T actual, T tolerance, T min, T max) {
  T errorBound = (max - min) / 2.0;
  T error = frc::InputModulus<T>(expected - actual, -errorBound, errorBound);

  if constexpr (std::is_arithmetic_v<T>) {
    return std::abs(error) < tolerance;
  } else {
    return units::math::abs(error) < tolerance;
  }
}

/**
 * Wraps an angle to the range -π to π radians (-180 to 180 degrees).
 *
 * @param angle Angle to wrap.
 */
WPILIB_DLLEXPORT
constexpr units::radian_t AngleModulus(units::radian_t angle) {
  return InputModulus<units::radian_t>(angle,
                                       units::radian_t{-std::numbers::pi},
                                       units::radian_t{std::numbers::pi});
}

// floorDiv and floorMod algorithms taken from Java

/**
 * Returns the largest (closest to positive infinity)
 * {@code int} value that is less than or equal to the algebraic quotient.
 *
 * @param x the dividend
 * @param y the divisor
 * @return the largest (closest to positive infinity)
 *   {@code int} value that is less than or equal to the algebraic quotient.
 */
constexpr std::signed_integral auto FloorDiv(std::signed_integral auto x,
                                             std::signed_integral auto y) {
  auto quot = x / y;
  auto rem = x % y;
  // if the signs are different and modulo not zero, round down
  if ((x < 0) != (y < 0) && rem != 0) {
    --quot;
  }
  return quot;
}

/**
 * Returns the floor modulus of the {@code int} arguments.
 * <p>
 * The floor modulus is {@code r = x - (floorDiv(x, y) * y)},
 * has the same sign as the divisor {@code y} or is zero, and
 * is in the range of {@code -std::abs(y) < r < +std::abs(y)}.
 *
 * @param x the dividend
 * @param y the divisor
 * @return the floor modulus {@code x - (floorDiv(x, y) * y)}
 */
constexpr std::signed_integral auto FloorMod(std::signed_integral auto x,
                                             std::signed_integral auto y) {
  return x - FloorDiv(x, y) * y;
}
}  // namespace frc
