// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "units/velocity.h"

namespace frc {
/**
 * Represents the wheel speeds for a mecanum drive drivetrain.
 */
struct MecanumDriveWheelSpeeds {
  /**
   * Speed of the front-left wheel.
   */
  units::meters_per_second_t frontLeft = 0_mps;

  /**
   * Speed of the front-right wheel.
   */
  units::meters_per_second_t frontRight = 0_mps;

  /**
   * Speed of the rear-left wheel.
   */
  units::meters_per_second_t rearLeft = 0_mps;

  /**
   * Speed of the rear-right wheel.
   */
  units::meters_per_second_t rearRight = 0_mps;

  /**
   * Normalizes the wheel speeds using some max attainable speed. Sometimes,
   * after inverse kinematics, the requested speed from a/several modules may be
   * above the max attainable speed for the driving motor on that module. To fix
   * this issue, one can "normalize" all the wheel speeds to make sure that all
   * requested module speeds are below the absolute threshold, while maintaining
   * the ratio of speeds between modules.
   *
   * @param attainableMaxSpeed The absolute max speed that a wheel can reach.
   */
  void Normalize(units::meters_per_second_t attainableMaxSpeed);
};
}  // namespace frc
