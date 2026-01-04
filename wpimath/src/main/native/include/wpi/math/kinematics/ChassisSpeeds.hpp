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
 * Represents the speed of a robot chassis. Although this struct contains
 * similar members compared to a Twist2d, they do NOT represent the same thing.
 * Whereas a Twist2d represents a change in pose w.r.t to the robot frame of
 * reference, a ChassisSpeeds struct represents a robot's velocity.
 *
 * A strictly non-holonomic drivetrain, such as a differential drive, should
 * never have a dy component because it can never move sideways. Holonomic
 * drivetrains such as swerve and mecanum will often have all three components.
 */
struct WPILIB_DLLEXPORT ChassisSpeeds {
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
   * Creates a Twist2d from ChassisSpeeds.
   *
   * @param dt The duration of the timestep.
   *
   * @return Twist2d.
   */
  constexpr Twist2d ToTwist2d(wpi::units::second_t dt) const {
    return Twist2d{vx * dt, vy * dt, omega * dt};
  }

  /**
   * Discretizes a continuous-time chassis speed.
   *
   * This function converts a continuous-time chassis speed into a discrete-time
   * one such that when the discrete-time chassis speed is applied for one
   * timestep, the robot moves as if the velocity components are independent
   * (i.e., the robot moves v_x * dt along the x-axis, v_y * dt along the
   * y-axis, and omega * dt around the z-axis).
   *
   * This is useful for compensating for translational skew when translating and
   * rotating a holonomic (swerve or mecanum) drivetrain. However, scaling down
   * the ChassisSpeeds after discretizing (e.g., when desaturating swerve module
   * speeds) rotates the direction of net motion in the opposite direction of
   * rotational velocity, introducing a different translational skew which is
   * not accounted for by discretization.
   *
   * @param dt The duration of the timestep the speeds should be applied for.
   * @return Discretized ChassisSpeeds.
   */
  constexpr ChassisSpeeds Discretize(wpi::units::second_t dt) const {
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
   * Converts this field-relative set of speeds into a robot-relative
   * ChassisSpeeds object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisSpeeds object representing the speeds in the robot's frame
   *     of reference.
   */
  constexpr ChassisSpeeds ToRobotRelative(const Rotation2d& robotAngle) const {
    // CW rotation into chassis frame
    auto rotated = Translation2d{wpi::units::meter_t{vx.value()},
                                 wpi::units::meter_t{vy.value()}}
                       .RotateBy(-robotAngle);
    return {wpi::units::meters_per_second_t{rotated.X().value()},
            wpi::units::meters_per_second_t{rotated.Y().value()}, omega};
  }

  /**
   * Converts this robot-relative set of speeds into a field-relative
   * ChassisSpeeds object.
   *
   * @param robotAngle The angle of the robot as measured by a gyroscope. The
   *     robot's angle is considered to be zero when it is facing directly away
   *     from your alliance station wall. Remember that this should be CCW
   *     positive.
   * @return ChassisSpeeds object representing the speeds in the field's frame
   *     of reference.
   */
  constexpr ChassisSpeeds ToFieldRelative(const Rotation2d& robotAngle) const {
    // CCW rotation out of chassis frame
    auto rotated = Translation2d{wpi::units::meter_t{vx.value()},
                                 wpi::units::meter_t{vy.value()}}
                       .RotateBy(robotAngle);
    return {wpi::units::meters_per_second_t{rotated.X().value()},
            wpi::units::meters_per_second_t{rotated.Y().value()}, omega};
  }

  /**
   * Adds two ChassisSpeeds and returns the sum.
   *
   * <p>For example, ChassisSpeeds{1.0, 0.5, 1.5} + ChassisSpeeds{2.0, 1.5, 0.5}
   * = ChassisSpeeds{3.0, 2.0, 2.0}
   *
   * @param other The ChassisSpeeds to add.
   *
   * @return The sum of the ChassisSpeeds.
   */
  constexpr ChassisSpeeds operator+(const ChassisSpeeds& other) const {
    return {vx + other.vx, vy + other.vy, omega + other.omega};
  }

  /**
   * Subtracts the other ChassisSpeeds from the current ChassisSpeeds and
   * returns the difference.
   *
   * <p>For example, ChassisSpeeds{5.0, 4.0, 2.0} - ChassisSpeeds{1.0, 2.0, 1.0}
   * = ChassisSpeeds{4.0, 2.0, 1.0}
   *
   * @param other The ChassisSpeeds to subtract.
   *
   * @return The difference between the two ChassisSpeeds.
   */
  constexpr ChassisSpeeds operator-(const ChassisSpeeds& other) const {
    return *this + -other;
  }

  /**
   * Returns the inverse of the current ChassisSpeeds.
   * This is equivalent to negating all components of the ChassisSpeeds.
   *
   * @return The inverse of the current ChassisSpeeds.
   */
  constexpr ChassisSpeeds operator-() const { return {-vx, -vy, -omega}; }

  /**
   * Multiplies the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   *
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} * 2
   * = ChassisSpeeds{4.0, 5.0, 1.0}
   *
   * @param scalar The scalar to multiply by.
   *
   * @return The scaled ChassisSpeeds.
   */
  constexpr ChassisSpeeds operator*(double scalar) const {
    return {scalar * vx, scalar * vy, scalar * omega};
  }

  /**
   * Divides the ChassisSpeeds by a scalar and returns the new ChassisSpeeds.
   *
   * <p>For example, ChassisSpeeds{2.0, 2.5, 1.0} / 2
   * = ChassisSpeeds{1.0, 1.25, 0.5}
   *
   * @param scalar The scalar to divide by.
   *
   * @return The scaled ChassisSpeeds.
   */
  constexpr ChassisSpeeds operator/(double scalar) const {
    return operator*(1.0 / scalar);
  }

  /**
   * Compares the ChassisSpeeds with another ChassisSpeed.
   *
   * @param other The other ChassisSpeeds.
   *
   * @return The result of the comparison. Is true if they are the same.
   */
  constexpr bool operator==(const ChassisSpeeds& other) const = default;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const ChassisSpeeds& speeds);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, ChassisSpeeds& speeds);

}  // namespace wpi::math

#include "wpi/math/kinematics/proto/ChassisSpeedsProto.hpp"
#include "wpi/math/kinematics/struct/ChassisSpeedsStruct.hpp"
