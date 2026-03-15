// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>

#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel velocities for a mecanum drive drivetrain.
 */
struct WPILIB_DLLEXPORT MecanumDriveWheelVelocities {
  /**
   * Velocity of the front-left wheel.
   */
  wpi::units::meters_per_second_t frontLeft = 0_mps;

  /**
   * Velocity of the front-right wheel.
   */
  wpi::units::meters_per_second_t frontRight = 0_mps;

  /**
   * Velocity of the rear-left wheel.
   */
  wpi::units::meters_per_second_t rearLeft = 0_mps;

  /**
   * Velocity of the rear-right wheel.
   */
  wpi::units::meters_per_second_t rearRight = 0_mps;

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the
   * specified maximum.
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
   * @return Desaturated MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities Desaturate(
      wpi::units::meters_per_second_t attainableMaxVelocity) const {
    std::array<wpi::units::meters_per_second_t, 4> wheelVelocities{
        frontLeft, frontRight, rearLeft, rearRight};
    wpi::units::meters_per_second_t realMaxVelocity =
        wpi::units::math::abs(*std::max_element(
            wheelVelocities.begin(), wheelVelocities.end(),
            [](const auto& a, const auto& b) {
              return wpi::units::math::abs(a) < wpi::units::math::abs(b);
            }));

    if (realMaxVelocity > attainableMaxVelocity) {
      for (int i = 0; i < 4; ++i) {
        wheelVelocities[i] =
            wheelVelocities[i] / realMaxVelocity * attainableMaxVelocity;
      }
      return MecanumDriveWheelVelocities{wheelVelocities[0], wheelVelocities[1],
                                         wheelVelocities[2],
                                         wheelVelocities[3]};
    }

    return *this;
  }

  /**
   * Adds two MecanumDriveWheelVelocities and returns the sum.
   *
   * For example, MecanumDriveWheelVelocities{1.0, 0.5, 2.0, 1.5} +
   * MecanumDriveWheelVelocities{2.0, 1.5, 0.5, 1.0} =
   * MecanumDriveWheelVelocities{3.0, 2.0, 2.5, 2.5}
   *
   * @param other The MecanumDriveWheelVelocities to add.
   * @return The sum of the MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities operator+(
      const MecanumDriveWheelVelocities& other) const {
    return {frontLeft + other.frontLeft, frontRight + other.frontRight,
            rearLeft + other.rearLeft, rearRight + other.rearRight};
  }

  /**
   * Subtracts the other MecanumDriveWheelVelocities from the current
   * MecanumDriveWheelVelocities and returns the difference.
   *
   * For example, MecanumDriveWheelVelocities{5.0, 4.0, 6.0, 2.5} -
   * MecanumDriveWheelVelocities{1.0, 2.0, 3.0, 0.5} =
   * MecanumDriveWheelVelocities{4.0, 2.0, 3.0, 2.0}
   *
   * @param other The MecanumDriveWheelVelocities to subtract.
   * @return The difference between the two MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities operator-(
      const MecanumDriveWheelVelocities& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current MecanumDriveWheelVelocities.
   * This is equivalent to negating all components of the
   * MecanumDriveWheelVelocities.
   *
   * @return The inverse of the current MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities operator-() const {
    return {-frontLeft, -frontRight, -rearLeft, -rearRight};
  }

  /**
   * Multiplies the MecanumDriveWheelVelocities by a scalar and returns the new
   * MecanumDriveWheelVelocities.
   *
   * For example, MecanumDriveWheelVelocities{2.0, 2.5, 3.0, 3.5} * 2 =
   * MecanumDriveWheelVelocities{4.0, 5.0, 6.0, 7.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities operator*(double scalar) const {
    return {scalar * frontLeft, scalar * frontRight, scalar * rearLeft,
            scalar * rearRight};
  }

  /**
   * Divides the MecanumDriveWheelVelocities by a scalar and returns the new
   * MecanumDriveWheelVelocities.
   *
   * For example, MecanumDriveWheelVelocities{2.0, 2.5, 1.5, 1.0} / 2 =
   * MecanumDriveWheelVelocities{1.0, 1.25, 0.75, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled MecanumDriveWheelVelocities.
   */
  constexpr MecanumDriveWheelVelocities operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/MecanumDriveWheelVelocitiesProto.hpp"
#include "wpi/math/kinematics/struct/MecanumDriveWheelVelocitiesStruct.hpp"
