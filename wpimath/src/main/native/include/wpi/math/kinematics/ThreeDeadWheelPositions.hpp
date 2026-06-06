// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/length.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel positions for a triple follower wheel drivetrain.
 */
struct WPILIB_DLLEXPORT ThreeDeadWheelPositions {
  /**
   * Constructs a ThreeDeadWheelPositions object with zeroed distances.
   */
  constexpr ThreeDeadWheelPositions() = default;

  /**
   * Constructs a ThreeDeadWheelPositions object with provided distances.
   *
   * @param x1 The distance traveled by the first forward-facing wheel.
   * @param x2 The distance traveled by the second forward-facing wheel.
   * @param y The distance traveled by the left-facing wheel.
   */
  constexpr ThreeDeadWheelPositions(wpi::units::meter_t x1,
                                         wpi::units::meter_t x2,
                                         wpi::units::meter_t y)
      : x1(x1), x2(x2), y(y) {}

  /**
   * Distance traveled by the first forward-facing wheel.
   */
  wpi::units::meter_t x1 = 0_m;

  /**
   * Distance traveled by the second forward-facing wheel.
   */
  wpi::units::meter_t x2 = 0_m;

  /**
   * Distance traveled by the left-facing wheel.
   */
  wpi::units::meter_t y = 0_m;

  /**
   * Checks equality between this ThreeDeadWheelPositions and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const ThreeDeadWheelPositions& other) const =
      default;

  constexpr ThreeDeadWheelPositions Interpolate(
      const ThreeDeadWheelPositions& endValue, double t) const {
    return {wpi::util::Lerp(x1, endValue.x1, t),
            wpi::util::Lerp(x2, endValue.x2, t),
            wpi::util::Lerp(y, endValue.y, t)};
  }
};
}  // namespace wpi::math
