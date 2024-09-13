// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "units/velocity.h"

namespace frc {
/**
 * Represents the wheel speeds for a differential drive drivetrain.
 */
struct WPILIB_DLLEXPORT DifferentialDriveWheelSpeeds {
  /**
   * Speed of the left side of the robot.
   */
  units::meters_per_second_t left = 0_mps;

  /**
   * Speed of the right side of the robot.
   */
  units::meters_per_second_t right = 0_mps;

  /**
   * Renormalizes the wheel speeds if either side is above the specified
   * maximum.
   *
   * Sometimes, after inverse kinematics, the requested speed from one or more
   * wheels may be above the max attainable speed for the driving motor on that
   * wheel. To fix this issue, one can reduce all the wheel speeds to make sure
   * that all requested module speeds are at-or-below the absolute threshold,
   * while maintaining the ratio of speeds between wheels.
   *
   * @param attainableMaxSpeed The absolute max speed that a wheel can reach.
   */
  void Desaturate(units::meters_per_second_t attainableMaxSpeed);

  /**
   * Adds two DifferentialDriveWheelSpeeds and returns the sum.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{1.0, 0.5} +
   * DifferentialDriveWheelSpeeds{2.0, 1.5} =
   * DifferentialDriveWheelSpeeds{3.0, 2.0}
   *
   * @param other The DifferentialDriveWheelSpeeds to add.
   *
   * @return The sum of the DifferentialDriveWheelSpeeds.
   */
  constexpr DifferentialDriveWheelSpeeds operator+(
      const DifferentialDriveWheelSpeeds& other) const {
    return {left + other.left, right + other.right};
  }

  /**
   * Subtracts the other DifferentialDriveWheelSpeeds from the current
   * DifferentialDriveWheelSpeeds and returns the difference.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{5.0, 4.0} -
   * DifferentialDriveWheelSpeeds{1.0, 2.0} =
   * DifferentialDriveWheelSpeeds{4.0, 2.0}
   *
   * @param other The DifferentialDriveWheelSpeeds to subtract.
   *
   * @return The difference between the two DifferentialDriveWheelSpeeds.
   */
  constexpr DifferentialDriveWheelSpeeds operator-(
      const DifferentialDriveWheelSpeeds& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current DifferentialDriveWheelSpeeds.
   * This is equivalent to negating all components of the
   * DifferentialDriveWheelSpeeds.
   *
   * @return The inverse of the current DifferentialDriveWheelSpeeds.
   */
  constexpr DifferentialDriveWheelSpeeds operator-() const {
    return {-left, -right};
  }

  /**
   * Multiplies the DifferentialDriveWheelSpeeds by a scalar and returns the new
   * DifferentialDriveWheelSpeeds.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{2.0, 2.5} * 2
   * = DifferentialDriveWheelSpeeds{4.0, 5.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled DifferentialDriveWheelSpeeds.
   */
  constexpr DifferentialDriveWheelSpeeds operator*(double scalar) const {
    return {scalar * left, scalar * right};
  }

  /**
   * Divides the DifferentialDriveWheelSpeeds by a scalar and returns the new
   * DifferentialDriveWheelSpeeds.
   *
   * <p>For example, DifferentialDriveWheelSpeeds{2.0, 2.5} / 2
   * = DifferentialDriveWheelSpeeds{1.0, 1.25}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled DifferentialDriveWheelSpeeds.
   */
  constexpr DifferentialDriveWheelSpeeds operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }
};
}  // namespace frc

#include "frc/kinematics/proto/DifferentialDriveWheelSpeedsProto.h"
#include "frc/kinematics/struct/DifferentialDriveWheelSpeedsStruct.h"
