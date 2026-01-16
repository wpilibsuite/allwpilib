// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents the state of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModuleState {
  /**
   * Speed of the wheel of the module.
   */
  wpi::units::meters_per_second_t speed = 0_mps;

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
    return wpi::units::math::abs(speed - other.speed) < 1E-9_mps &&
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
  [[nodiscard]]
  constexpr SwerveModuleState Optimize(const Rotation2d& currentAngle) {
    auto delta = angle - currentAngle;
    if (wpi::units::math::abs(delta.Degrees()) > 90_deg) {
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
  [[nodiscard]]
  constexpr SwerveModuleState CosineScale(const Rotation2d& currentAngle) {
    return {speed * (angle - currentAngle).Cos(), angle};
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/SwerveModuleStateProto.hpp"
#include "wpi/math/kinematics/struct/SwerveModuleStateStruct.hpp"
