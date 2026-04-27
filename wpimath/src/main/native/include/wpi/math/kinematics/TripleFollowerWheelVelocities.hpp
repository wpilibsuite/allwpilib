// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/velocity.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel velocities for a triple follower wheel odometry setup.
 */
struct WPILIB_DLLEXPORT TripleFollowerWheelVelocities {
  /**
   * Velocity measured by the 1st forward-facing wheel.
   */
  wpi::units::meters_per_second_t vx1 = 0_mps;

  /**
   * Velocity measured by the 2nd forward-facing wheel.
   */
  wpi::units::meters_per_second_t vx2 = 0_mps;

  /**
   * Velocity measured by the left-facing wheel.
   */
  wpi::units::meters_per_second_t y = 0_mps;

  /**
   * Checks equality between this TripleFollowerWheelVelocities and another
   * object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const TripleFollowerWheelVelocities& other) const =
      default;

  constexpr TripleFollowerWheelVelocities Interpolate(
      const TripleFollowerWheelVelocities& endValue, double t) const {
    return {wpi::util::Lerp(vx1, endValue.vx1, t),
            wpi::util::Lerp(vx2, endValue.vx2, t),
            wpi::util::Lerp(y, endValue.y, t)};
  }
};
}  // namespace wpi::math
