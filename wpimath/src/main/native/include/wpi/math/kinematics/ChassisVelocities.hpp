// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Represents robot chassis velocities.
 *
 * Although this struct contains similar members compared to a Twist2d, they do
 * NOT represent the same thing. Whereas a Twist2d represents a change in pose
 * w.r.t to the robot frame of reference, a ChassisVelocities struct represents
 * a robot's velocities.
 *
 * A strictly non-holonomic drivetrain, such as a differential drive, should
 * never have a dy component because it can never move sideways. Holonomic
 * drivetrains such as swerve and mecanum will often have all three components.
 */
struct WPILIB_DLLEXPORT ChassisVelocities {
  /**
   * Velocity along the x-axis. (Fwd is +)
   */
  wpi::units::meters_per_second_t vx = 0_mps;

  /**
   * Velocity along the y-axis. (Left is +)
   */
  wpi::units::meters_per_second_t vy = 0_mps;

  /**
   * Represents the angular velocity of the robot frame. (CCW is +)
   */
  wpi::units::radians_per_second_t omega = 0_rad_per_s;

  /**
   * Creates a Twist2d from ChassisVelocities.
   *
   * @param dt The duration of the timestep.
   *
   * @return Twist2d.
   */
  constexpr Twist2d ToTwist2d(wpi::units::second_t dt) const {
    return Twist2d{vx * dt, vy * dt, omega * dt};
  }

  /**
   * Discretizes continuous-time chassis velocities.
   *
   * This function converts continuous-time chassis velocities into
   * discrete-time ones such that when the discrete-time chassis velocities are
   * applied for one timestep, the robot moves as if the velocity components are
   * independent (i.e., the robot moves v_x * dt along the x-axis, v_y * dt
   * along the y-axis, and omega * dt around the z-axis).
   *
   * This is useful for compensating for translational skew when translating and
   * rotating a holonomic (swerve or mecanum) drivetrain. However, scaling down
   * the ChassisVelocities after discretizing (e.g., when desaturating swerve
   * module velocities) rotates the direction of net motion in the opposite
   * direction of rotational velocity, introducing a different translational
   * skew which is not accounted for by discretization.
   *
   * @param dt The duration of the timestep the velocities should be applied
   *     for.
   * @return Discretized ChassisVelocities.
   */
  constexpr ChassisVelocities Discretize(wpi::units::second_t dt) const {
    // Construct the desired pose after a timestep, relative to the current
    // pose. The desired pose has decoupled translation and rotation.
    Transform2d desiredTransform{vx * dt, vy * dt, omega * dt};

    // Find the chassis translation/rotation deltas in the robot frame that move
    // the robot from its current pose to the desired pose
    auto twist = desiredTransform.Log();

    // Turn the chassis translation/rotation deltas into average velocities
    return {twist.dx / dt, twist.dy / dt, twist.dtheta / dt};
  }

  /**
   * Converts this field-relative set of velocities into a robot-relative
   * ChassisVelocities object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisVelocities object representing the velocities in the robot's
   *     frame of reference.
   */
  constexpr ChassisVelocities ToRobotRelative(
      const Rotation2d& robotAngle) const {
    // CW rotation into chassis frame
    auto rotated = Translation2d{wpi::units::meter_t{vx.value()},
                                 wpi::units::meter_t{vy.value()}}
                       .RotateBy(-robotAngle);
    return {wpi::units::meters_per_second_t{rotated.X().value()},
            wpi::units::meters_per_second_t{rotated.Y().value()}, omega};
  }

  /**
   * Converts this robot-relative set of velocities into a field-relative
   * ChassisVelocities object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisVelocities object representing the velocities in the field's
   *     frame of reference.
   */
  constexpr ChassisVelocities ToFieldRelative(
      const Rotation2d& robotAngle) const {
    // CCW rotation out of chassis frame
    auto rotated = Translation2d{wpi::units::meter_t{vx.value()},
                                 wpi::units::meter_t{vy.value()}}
                       .RotateBy(robotAngle);
    return {wpi::units::meters_per_second_t{rotated.X().value()},
            wpi::units::meters_per_second_t{rotated.Y().value()}, omega};
  }

  /**
   * Adds two ChassisVelocities and returns the sum.
   *
   * For example, ChassisVelocities{1.0, 0.5, 1.5} +
   * ChassisVelocities{2.0, 1.5, 0.5} = ChassisVelocities{3.0, 2.0, 2.0}
   *
   * @param other The ChassisVelocities to add.
   *
   * @return The sum of the ChassisVelocities.
   */
  constexpr ChassisVelocities operator+(const ChassisVelocities& other) const {
    return {vx + other.vx, vy + other.vy, omega + other.omega};
  }

  /**
   * Subtracts the other ChassisVelocities from the current ChassisVelocities
   * and returns the difference.
   *
   * For example, ChassisVelocities{5.0, 4.0, 2.0} -
   * ChassisVelocities{1.0, 2.0, 1.0} = ChassisVelocities{4.0, 2.0, 1.0}
   *
   * @param other The ChassisVelocities to subtract.
   *
   * @return The difference between the two ChassisVelocities.
   */
  constexpr ChassisVelocities operator-(const ChassisVelocities& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current ChassisVelocities.
   * This is equivalent to negating all components of the ChassisVelocities.
   *
   * @return The inverse of the current ChassisVelocities.
   */
  constexpr ChassisVelocities operator-() const { return {-vx, -vy, -omega}; }

  /**
   * Multiplies the ChassisVelocities by a scalar and returns the new
   * ChassisVelocities.
   *
   * For example, ChassisVelocities{2.0, 2.5, 1.0} * 2
   * = ChassisVelocities{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled ChassisVelocities.
   */
  constexpr ChassisVelocities operator*(double scalar) const {
    return {scalar * vx, scalar * vy, scalar * omega};
  }

  /**
   * Divides the ChassisVelocities by a scalar and returns the new
   * ChassisVelocities.
   *
   * For example, ChassisVelocities{2.0, 2.5, 1.0} / 2
   * = ChassisVelocities{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled ChassisVelocities.
   */
  constexpr ChassisVelocities operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Compares the ChassisVelocities with another ChassisVelocity.
   *
   * @param other The other ChassisVelocities.
   *
   * @return The result of the comparison. Is true if they are the same.
   */
  constexpr bool operator==(const ChassisVelocities& other) const = default;
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/ChassisVelocitiesProto.hpp"
#include "wpi/math/kinematics/struct/ChassisVelocitiesStruct.hpp"
