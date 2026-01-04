// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel velocities for a differential drive drivetrain.
 */
struct WPILIB_DLLEXPORT DifferentialDriveWheelVelocities {
  /**
   * Velocity of the left side of the robot.
   */
  wpi::units::meters_per_second_t left = 0_mps;

  /**
   * Velocity of the right side of the robot.
   */
  wpi::units::meters_per_second_t right = 0_mps;

  /**
   * Renormalizes the wheel velocities if either side is above the specified
   * maximum.
   *
   * Sometimes, after inverse kinematics, the requested velocity from one or
   * more wheels may be above the max attainable velocity for the driving motor
   * on that wheel. To fix this issue, one can reduce all the wheel velocities
   * to make sure that all requested module velocities are at-or-below the
   * absolute threshold, while maintaining the ratio of velocities between
   * wheels.
   *
   * @param attainableMaxVelocity The absolute max velocity that a wheel can
   *     reach.
   */
  constexpr void Desaturate(
      wpi::units::meters_per_second_t attainableMaxVelocity) {
    auto realMaxVelocity = wpi::units::math::max(wpi::units::math::abs(left),
                                                 wpi::units::math::abs(right));

    if (realMaxVelocity > attainableMaxVelocity) {
      left = left / realMaxVelocity * attainableMaxVelocity;
      right = right / realMaxVelocity * attainableMaxVelocity;
    }
  }

  /**
   * Adds two DifferentialDriveWheelVelocities and returns the sum.
   *
   * For example, DifferentialDriveWheelVelocities{1.0, 0.5} +
   * DifferentialDriveWheelVelocities{2.0, 1.5} =
   * DifferentialDriveWheelVelocities{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelVelocities to add.
   * @return The sum of the DifferentialDriveWheelVelocities.
   */
  constexpr DifferentialDriveWheelVelocities operator+(
      const DifferentialDriveWheelVelocities& other) const {
    return {left + other.left, right + other.right};
  }

  /**
   * Subtracts the other DifferentialDriveWheelVelocities from the current
   * DifferentialDriveWheelVelocities and returns the difference.
   *
   * For example, DifferentialDriveWheelVelocities{5.0, 4.0} -
   * DifferentialDriveWheelVelocities{1.0, 2.0} =
   * DifferentialDriveWheelVelocities{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelVelocities to subtract.
   * @return The difference between the two DifferentialDriveWheelVelocities.
   */
  constexpr DifferentialDriveWheelVelocities operator-(
      const DifferentialDriveWheelVelocities& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelVelocities.
   * This is equivalent to negating all components of the
   * DifferentialDriveWheelVelocities.
   *
   * @return The inverse of the current DifferentialDriveWheelVelocities.
   */
  constexpr DifferentialDriveWheelVelocities operator-() const {
    return {-left, -right};
  }

  /**
   * Multiplies the DifferentialDriveWheelVelocities by a scalar and returns the
   * new DifferentialDriveWheelVelocities.
   *
   * For example, DifferentialDriveWheelVelocities{2.0, 2.5} * 2
   * = DifferentialDriveWheelVelocities{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled DifferentialDriveWheelVelocities.
   */
  constexpr DifferentialDriveWheelVelocities operator*(double scalar) const {
    return {scalar * left, scalar * right};
  }

  /**
   * Divides the DifferentialDriveWheelVelocities by a scalar and returns the
   * new DifferentialDriveWheelVelocities.
   *
   * For example, DifferentialDriveWheelVelocities{2.0, 2.5} / 2
   * = DifferentialDriveWheelVelocities{1.0, 1.25}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled DifferentialDriveWheelVelocities.
   */
  constexpr DifferentialDriveWheelVelocities operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/DifferentialDriveWheelVelocitiesProto.hpp"
#include "wpi/math/kinematics/struct/DifferentialDriveWheelVelocitiesStruct.hpp"
