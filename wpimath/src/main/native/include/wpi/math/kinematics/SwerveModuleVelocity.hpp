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
 * Represents the velocity of one swerve module.
 */
struct WPILIB_DLLEXPORT SwerveModuleVelocity {
  /**
   * Velocity of the wheel of the module.
   */
  wpi::units::meters_per_second_t velocity = 0_mps;

  /**
   * Angle of the module.
   */
  Rotation2d angle;

  /**
   * Checks equality between this SwerveModuleVelocity and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const SwerveModuleVelocity& other) const {
    return wpi::units::math::abs(velocity - other.velocity) < 1E-9_mps &&
           angle == other.angle;
  }

  /**
   * Minimize the change in the heading this swerve module velocity would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * @param currentAngle The current module angle.
   */
  constexpr void Optimize(const Rotation2d& currentAngle) {
    auto delta = angle - currentAngle;
    if (wpi::units::math::abs(delta.Degrees()) > 90_deg) {
      velocity *= -1;
      angle = angle + Rotation2d{180_deg};
    }
  }

  /**
   * Minimize the change in heading the desired swerve module velocity would
   * require by potentially reversing the direction the wheel spins. If this is
   * used with the PIDController class's continuous input functionality, the
   * furthest a wheel will ever rotate is 90 degrees.
   *
   * @param desiredVelocity The desired velocity.
   * @param currentAngle The current module angle.
   */
  [[deprecated("Use instance method instead.")]]
  constexpr static SwerveModuleVelocity Optimize(
      const SwerveModuleVelocity& desiredVelocity,
      const Rotation2d& currentAngle) {
    auto delta = desiredVelocity.angle - currentAngle;
    if (wpi::units::math::abs(delta.Degrees()) > 90_deg) {
      return {-desiredVelocity.velocity,
              desiredVelocity.angle + Rotation2d{180_deg}};
    } else {
      return {desiredVelocity.velocity, desiredVelocity.angle};
    }
  }

  /**
   * Scales velocity by cosine of angle error. This scales down movement
   * perpendicular to the desired direction of travel that can occur when
   * modules change directions. This results in smoother driving.
   *
   * @param currentAngle The current module angle.
   */
  constexpr void CosineScale(const Rotation2d& currentAngle) {
    velocity *= (angle - currentAngle).Cos();
  }
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/SwerveModuleVelocityProto.hpp"
#include "wpi/math/kinematics/struct/SwerveModuleVelocityStruct.hpp"
