// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/length.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel positions for a triple follower wheel odometry setup.
 */
struct WPILIB_DLLEXPORT TripleFollowerWheelPositions {
  /**
   * Distance measured by the 1st forward-facing wheel.
   */
  wpi::units::meter_t x1 = 0_m;

  /**
   * Distance measured by the 2nd forward-facing wheel.
   */
  wpi::units::meter_t x2 = 0_m;

  /**
   * Distance measured by the left-facing wheel.
   */
  wpi::units::meter_t y = 0_m;

  /**
   * Checks equality between this TripleFollowerWheelPositions and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const TripleFollowerWheelPositions& other) const =
      default;

  constexpr TripleFollowerWheelPositions Interpolate(
      const TripleFollowerWheelPositions& endValue, double t) const {
    return {wpi::util::Lerp(x1, endValue.x1, t),
            wpi::util::Lerp(x2, endValue.x2, t),
            wpi::util::Lerp(y, endValue.y, t)};
  }
};
}  // namespace wpi::math
