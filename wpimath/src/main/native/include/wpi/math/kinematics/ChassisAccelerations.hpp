// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/util/SymbolExports.hpp>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"

namespace wpi::math {
/**
 * Represents the acceleration of a robot chassis. Although this struct contains
 * similar members compared to a ChassisSpeeds, they do NOT represent the same
 * thing. Whereas a ChassisSpeeds object represents a robot's velocity, a
 * ChassisAccelerations object represents a robot's acceleration.
 *
 * A strictly non-holonomic drivetrain, such as a differential drive, should
 * never have an ay component because it can never move sideways. Holonomic
 * drivetrains such as swerve and mecanum will often have all three components.
 */
struct WPILIB_DLLEXPORT ChassisAccelerations {
  /**
   * Acceleration along the x-axis. (Fwd is +)
   */
  units::meters_per_second_squared_t ax = 0_mps_sq;

  /**
   * Acceleration along the y-axis. (Left is +)
   */
  units::meters_per_second_squared_t ay = 0_mps_sq;

  /**
   * Angular acceleration of the robot frame. (CCW is +)
   */
  units::radians_per_second_squared_t alpha = 0_rad_per_s_sq;

  /**
   * Converts this field-relative set of accelerations into a robot-relative
   * ChassisAccelerations object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisAccelerations object representing the accelerations in the
   *     robot's frame of reference.
   */
  constexpr ChassisAccelerations ToRobotRelative(
      const Rotation2d& robotAngle) const {
    // CW rotation into chassis frame
    auto rotated =
        Translation2d{units::meter_t{ax.value()}, units::meter_t{ay.value()}}
            .RotateBy(-robotAngle);
    return {units::meters_per_second_squared_t{rotated.X().value()},
            units::meters_per_second_squared_t{rotated.Y().value()}, alpha};
  }

  /**
   * Converts this robot-relative set of accelerations into a field-relative
   * ChassisAccelerations object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisAccelerations object representing the accelerations in the
   *     field's frame of reference.
   */
  constexpr ChassisAccelerations ToFieldRelative(
      const Rotation2d& robotAngle) const {
    // CCW rotation out of chassis frame
    auto rotated =
        Translation2d{units::meter_t{ax.value()}, units::meter_t{ay.value()}}
            .RotateBy(robotAngle);
    return {units::meters_per_second_squared_t{rotated.X().value()},
            units::meters_per_second_squared_t{rotated.Y().value()}, alpha};
  }

  /**
   * Adds two ChassisAccelerations and returns the sum.
   *
   * <p>For example, ChassisAccelerations{1.0, 0.5, 1.5} +
   * ChassisAccelerations{2.0, 1.5, 0.5} = ChassisAccelerations{3.0, 2.0, 2.0}
   *
   * @param other The ChassisAccelerations to add.
   *
   * @return The sum of the ChassisAccelerations.
   */
  constexpr ChassisAccelerations operator+(
      const ChassisAccelerations& other) const {
    return {ax + other.ax, ay + other.ay, alpha + other.alpha};
  }

  /**
   * Subtracts the other ChassisAccelerations from the current
   * ChassisAccelerations and returns the difference.
   *
   * <p>For example, ChassisAccelerations{5.0, 4.0, 2.0} -
   * ChassisAccelerations{1.0, 2.0, 1.0} = ChassisAccelerations{4.0, 2.0, 1.0}
   *
   * @param other The ChassisAccelerations to subtract.
   *
   * @return The difference between the two ChassisAccelerations.
   */
  constexpr ChassisAccelerations operator-(
      const ChassisAccelerations& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current ChassisAccelerations.
   * This is equivalent to negating all components of the ChassisAccelerations.
   *
   * @return The inverse of the current ChassisAccelerations.
   */
  constexpr ChassisAccelerations operator-() const {
    return {-ax, -ay, -alpha};
  }

  /**
   * Multiplies the ChassisAccelerations by a scalar and returns the new
   * ChassisAccelerations.
   *
   * <p>For example, ChassisAccelerations{2.0, 2.5, 1.0} * 2
   * = ChassisAccelerations{4.0, 5.0, 2.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled ChassisAccelerations.
   */
  constexpr ChassisAccelerations operator*(double scalar) const {
    return {scalar * ax, scalar * ay, scalar * alpha};
  }

  /**
   * Divides the ChassisAccelerations by a scalar and returns the new
   * ChassisAccelerations.
   *
   * <p>For example, ChassisAccelerations{2.0, 2.5, 1.0} / 2
   * = ChassisAccelerations{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled ChassisAccelerations.
   */
  constexpr ChassisAccelerations operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Compares the ChassisAccelerations with another ChassisAccelerations.
   *
   * @param other The other ChassisAccelerations.
   *
   * @return The result of the comparison. Is true if they are the same.
   */
  constexpr bool operator==(const ChassisAccelerations& other) const = default;
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/ChassisAccelerationsProto.hpp"
#include "wpi/math/kinematics/struct/ChassisAccelerationsStruct.hpp"
