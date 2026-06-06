// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/length.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel positions for a double follower wheel drivetrain.
 */
struct WPILIB_DLLEXPORT TwoDeadWheelPositions {
  /**
   * Constructs a TwoDeadWheelPositions object with zeroed distances.
   */
  constexpr TwoDeadWheelPositions() = default;

  /**
   * Constructs a TwoDeadWheelPositions object with provided distances.
   *
   * @param x The distance traveled by the forward-facing wheel.
   * @param y The distance traveled by the left-facing wheel.
   */
  constexpr TwoDeadWheelPositions(wpi::units::meter_t x,
                                         wpi::units::meter_t y)
      : x(x), y(y) {}

  /**
   * Distance traveled by the forward-facing wheel.
   */
  wpi::units::meter_t x = 0_m;

  /**
   * Distance traveled by the left-facing wheel.
   */
  wpi::units::meter_t y = 0_m;

  /**
   * Checks equality between this TwoDeadWheelPositions and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const TwoDeadWheelPositions& other) const =
      default;

  constexpr TwoDeadWheelPositions Interpolate(
      const TwoDeadWheelPositions& endValue, double t) const {
    return {wpi::util::Lerp(x, endValue.x, t),
            wpi::util::Lerp(y, endValue.y, t)};
  }
};
}  // namespace wpi::math
