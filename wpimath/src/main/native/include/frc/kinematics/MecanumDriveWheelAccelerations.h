// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include <wpi/SymbolExports.h>

#include "units/acceleration.h"
#include "units/math.h"

namespace frc {
/**
 * Represents the wheel accelerations for a mecanum drive drivetrain.
 */
struct WPILIB_DLLEXPORT MecanumDriveWheelAccelerations {
  /**
   * Acceleration of the front-left wheel.
   */
  units::meters_per_second_squared_t frontLeft = 0_mps_sq;

  /**
   * Acceleration of the front-right wheel.
   */
  units::meters_per_second_squared_t frontRight = 0_mps_sq;

  /**
   * Acceleration of the rear-left wheel.
   */
  units::meters_per_second_squared_t rearLeft = 0_mps_sq;

  /**
   * Acceleration of the rear-right wheel.
   */
  units::meters_per_second_squared_t rearRight = 0_mps_sq;

  /**
   * Adds two MecanumDriveWheelAccelerations and returns the sum.
   *
   * <p>For example, MecanumDriveWheelAccelerations{1.0, 0.5, 2.0, 1.5} +
   * MecanumDriveWheelAccelerations{2.0, 1.5, 0.5, 1.0} =
   * MecanumDriveWheelAccelerations{3.0, 2.0, 2.5, 2.5}
   *
   * @param other The MecanumDriveWheelAccelerations to add.
   * @return The sum of the MecanumDriveWheelAccelerations.
   */
  constexpr MecanumDriveWheelAccelerations operator+(
      const MecanumDriveWheelAccelerations& other) const {
    return {frontLeft + other.frontLeft, frontRight + other.frontRight,
            rearLeft + other.rearLeft, rearRight + other.rearRight};
  }

  /**
   * Subtracts the other MecanumDriveWheelAccelerations from the current
   * MecanumDriveWheelAccelerations and returns the difference.
   *
   * <p>For example, MecanumDriveWheelAccelerations{5.0, 4.0, 6.0, 2.5} -
   * MecanumDriveWheelAccelerations{1.0, 2.0, 3.0, 0.5} =
   * MecanumDriveWheelAccelerations{4.0, 2.0, 3.0, 2.0}
   *
   * @param other The MecanumDriveWheelAccelerations to subtract.
   * @return The difference between the two MecanumDriveWheelAccelerations.
   */
  constexpr MecanumDriveWheelAccelerations operator-(
      const MecanumDriveWheelAccelerations& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current MecanumDriveWheelAccelerations.
   * This is equivalent to negating all components of the
   * MecanumDriveWheelAccelerations.
   *
   * @return The inverse of the current MecanumDriveWheelAccelerations.
   */
  constexpr MecanumDriveWheelAccelerations operator-() const {
    return {-frontLeft, -frontRight, -rearLeft, -rearRight};
  }

  /**
   * Multiplies the MecanumDriveWheelAccelerations by a scalar and returns the new
   * MecanumDriveWheelAccelerations.
   *
   * <p>For example, MecanumDriveWheelAccelerations{2.0, 2.5, 3.0, 3.5} * 2 =
   * MecanumDriveWheelAccelerations{4.0, 5.0, 6.0, 7.0}
   *
   * @param scalar The scalar to multiply by.
   * @return The scaled MecanumDriveWheelAccelerations.
   */
  constexpr MecanumDriveWheelAccelerations operator*(double scalar) const {
    return {scalar * frontLeft, scalar * frontRight, scalar * rearLeft,
            scalar * rearRight};
  }

  /**
   * Divides the MecanumDriveWheelAccelerations by a scalar and returns the new
   * MecanumDriveWheelAccelerations.
   *
   * <p>For example, MecanumDriveWheelAccelerations{2.0, 2.5, 1.5, 1.0} / 2 =
   * MecanumDriveWheelAccelerations{1.0, 1.25, 0.75, 0.5}
   *
   * @param scalar The scalar to divide by.
   * @return The scaled MecanumDriveWheelAccelerations.
   */
  constexpr MecanumDriveWheelAccelerations operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Compares two MecanumDriveWheelAccelerations objects.
   *
   * @param other The other MecanumDriveWheelAccelerations.
   *
   * @return True if the MecanumDriveWheelAccelerations are equal.
   */
  constexpr bool operator==(const MecanumDriveWheelAccelerations& other) const = default;
};
}  // namespace frc

#include "frc/kinematics/proto/MecanumDriveWheelAccelerationsProto.h"
#include "frc/kinematics/struct/MecanumDriveWheelAccelerationsStruct.h"
