// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"
#include "units/math.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents the state of one swerve module.
 */
struct SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  units::meters_per_second_t speed = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Minimize the change in heading the desired swerve module state would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * @param desiredState The desired state.
   * @param currentAngle The current module angle.
   */
  static SwerveModuleState Optimize(const SwerveModuleState& desiredState,
                                    const Rotation2d& currentAngle) {
    auto delta = desiredState.angle - currentAngle;
    if (units::math::abs(delta.Degrees()) > 90_deg) {
      return {-desiredState.speed, desiredState.angle + Rotation2d{180_deg}};
    } else {
      return {desiredState.speed, desiredState.angle};
    }
  }
};
}  // namespace frc
