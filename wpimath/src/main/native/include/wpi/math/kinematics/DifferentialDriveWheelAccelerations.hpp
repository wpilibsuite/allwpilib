// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/units/acceleration.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the wheel accelerations for a differential drive drivetrain.
 */
struct WPILIB_DLLEXPORT DifferentialDriveWheelAccelerations {
  /**
   * Acceleration of the left side of the robot.
   */
  units::meters_per_second_squared_t left = 0_mps_sq;

  /**
   * Acceleration of the right side of the robot.
   */
  units::meters_per_second_squared_t right = 0_mps_sq;

  /**
   * Adds two DifferentialDriveWheelAccelerations and returns the sum.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{1.0, 0.5} +
   * DifferentialDriveWheelAccelerations{2.0, 1.5} =
   * DifferentialDriveWheelAccelerations{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelAccelerations to add.
   *
   * @return The sum of the DifferentialDriveWheelAccelerations.
   */
  constexpr DifferentialDriveWheelAccelerations operator+(
      const DifferentialDriveWheelAccelerations& other) const {
    return {left + other.left, right + other.right};
  }

  /**
   * Subtracts the other DifferentialDriveWheelAccelerations from the current
   * DifferentialDriveWheelAccelerations and returns the difference.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{5.0, 4.0} -
   * DifferentialDriveWheelAccelerations{1.0, 2.0} =
   * DifferentialDriveWheelAccelerations{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelAccelerations to subtract.
   *
   * @return The difference between the two DifferentialDriveWheelAccelerations.
   */
  constexpr DifferentialDriveWheelAccelerations operator-(
      const DifferentialDriveWheelAccelerations& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelAccelerations.
   * This is equivalent to negating all components of the
   * DifferentialDriveWheelAccelerations.
   *
   * @return The inverse of the current DifferentialDriveWheelAccelerations.
   */
  constexpr DifferentialDriveWheelAccelerations operator-() const {
    return {-left, -right};
  }

  /**
   * Multiplies the DifferentialDriveWheelAccelerations by a scalar and returns
   * the new DifferentialDriveWheelAccelerations.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{2.0, 2.5} * 2
   * = DifferentialDriveWheelAccelerations{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled DifferentialDriveWheelAccelerations.
   */
  constexpr DifferentialDriveWheelAccelerations operator*(double scalar) const {
    return {scalar * left, scalar * right};
  }

  /**
   * Divides the DifferentialDriveWheelAccelerations by a scalar and returns the
   * new DifferentialDriveWheelAccelerations.
   *
   * <p>For example, DifferentialDriveWheelAccelerations{2.0, 2.5} / 2
   * = DifferentialDriveWheelAccelerations{1.0, 1.25}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled DifferentialDriveWheelAccelerations.
   */
  constexpr DifferentialDriveWheelAccelerations operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Compares two DifferentialDriveWheelAccelerations objects.
   *
   * @param other The other DifferentialDriveWheelAccelerations.
   *
   * @return True if the DifferentialDriveWheelAccelerations are equal.
   */
  constexpr bool operator==(
      const DifferentialDriveWheelAccelerations& other) const = default;
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/DifferentialDriveWheelAccelerationsProto.hpp"
#include "wpi/math/kinematics/struct/DifferentialDriveWheelAccelerationsStruct.hpp"
