// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/acceleration.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel accelerations for a triple follower wheel odometry
 * setup.
 */
struct WPILIB_DLLEXPORT TripleFollowerWheelAccelerations {
  /**
   * Acceleration measured by the 1st forward-facing wheel.
   */
  wpi::units::meters_per_second_squared_t ax1 = 0_mps_sq;

  /**
   * Acceleration measured by the 2nd forward-facing wheel.
   */
  wpi::units::meters_per_second_squared_t ax2 = 0_mps_sq;

  /**
   * Acceleration measured by the left-facing wheel.
   */
  wpi::units::meters_per_second_squared_t ay = 0_mps_sq;

  /**
   * Checks equality between this TripleFollowerWheelAccelerations and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(
      const TripleFollowerWheelAccelerations& other) const = default;

  constexpr TripleFollowerWheelAccelerations Interpolate(
      const TripleFollowerWheelAccelerations& endValue, double t) const {
    return {wpi::util::Lerp(ax1, endValue.ax1, t),
            wpi::util::Lerp(ax2, endValue.ax2, t),
            wpi::util::Lerp(ay, endValue.ay, t)};
  }
};
}  // namespace wpi::math
