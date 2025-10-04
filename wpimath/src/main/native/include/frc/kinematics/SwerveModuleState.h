// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"
#include "units/math.h"
#include "units/velocity.h"

namespace frc {
/**
 * Represents the state of one swerve module.
 */
struct [[nodiscard]] WPILIB_DLLEXPORT SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  units::meters_per_second_t speed = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModuleState and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const SwerveModuleState& other) const {
    return units::math::abs(speed - other.speed) < 1E-9_mps &&
           angle == other.angle;
  }

  /**
   * Minimize the change in the heading this swerve module state would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * @param currentAngle The current module angle.
   * @return The optimized module state.
   */
  constexpr SwerveModuleState Optimize(const Rotation2d& currentAngle) {
    auto delta = angle - currentAngle;
    if (units::math::abs(delta.Degrees()) > 90_deg) {
      return {-speed, angle + Rotation2d{180_deg}};
    } else {
      return {speed, angle};
    }
  }

  /**
   * Scales speed by cosine of angle error. This scales down movement
   * perpendicular to the desired direction of travel that can occur when
   * modules change directions. This results in smoother driving.
   *
   * @param currentAngle The current module angle.
   * @return The scaled module state.
   */
  constexpr SwerveModuleState CosineScale(const Rotation2d& currentAngle) {
    return {speed * (angle - currentAngle).Cos(), angle};
  }
};
}  // namespace frc

#include "frc/kinematics/proto/SwerveModuleStateProto.h"
#include "frc/kinematics/struct/SwerveModuleStateStruct.h"
