// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual wheel speeds. Robot code should not use this directly-
 * Instead, use the particular type for your drivetrain (e.g.,
 * DifferentialDriveKinematics).
 *
 * Inverse kinematics converts a desired chassis speed into wheel speeds whereas
 * forward kinematics converts wheel speeds into chassis speed.
 */
template <typename WheelSpeeds, typename WheelPositions,
          typename WheelAccelerations>
  requires std::copy_constructible<WheelPositions> &&
           std::assignable_from<WheelPositions&, const WheelPositions&>
class WPILIB_DLLEXPORT Kinematics {
 public:
  virtual ~Kinematics() noexcept = default;

  /**
   * Performs forward kinematics to return the resulting chassis speed from the
   * wheel speeds. This method is often used for odometry -- determining the
   * robot's position on the field using data from the real-world speed of each
   * wheel on the robot.
   *
   * @param wheelSpeeds The speeds of the wheels.
   * @return The chassis speed.
   */
  virtual ChassisSpeeds ToChassisSpeeds(
      const WheelSpeeds& wheelSpeeds) const = 0;

  /**
   * Performs inverse kinematics to return the wheel speeds from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * wheel speeds.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @return The wheel speeds.
   */
  virtual WheelSpeeds ToWheelSpeeds(
      const ChassisSpeeds& chassisSpeeds) const = 0;

  /**
   * Performs forward kinematics to return the resulting Twist2d from the given
   * change in wheel positions. This method is often used for odometry --
   * determining the robot's position on the field using changes in the distance
   * driven by each wheel on the robot.
   *
   * @param start The starting distances driven by the wheels.
   * @param end The ending distances driven by the wheels.
   *
   * @return The resulting Twist2d in the robot's movement.
   */
  virtual Twist2d ToTwist2d(const WheelPositions& start,
                            const WheelPositions& end) const = 0;

  /**
   * Performs interpolation between two values.
   *
   * @param start The value to start at.
   * @param end The value to end at.
   * @param t How far between the two values to interpolate. This should be
   * bounded to [0, 1].
   * @return The interpolated value.
   */
  virtual WheelPositions Interpolate(const WheelPositions& start,
                                     const WheelPositions& end,
                                     double t) const = 0;

  /**
   * Performs forward kinematics to return the resulting chassis accelerations
   * from the wheel accelerations. This method is often used for dynamics
   * calculations -- determining the robot's acceleration on the field using
   * data from the real-world acceleration of each wheel on the robot.
   *
   * @param wheelAccelerations The accelerations of the wheels.
   * @return The chassis accelerations.
   */
  virtual ChassisAccelerations ToChassisAccelerations(
      const WheelAccelerations& wheelAccelerations) const = 0;

  /**
   * Performs inverse kinematics to return the wheel accelerations from a
   * desired chassis acceleration. This method is often used for dynamics
   * calculations -- converting desired robot accelerations into individual
   * wheel accelerations.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @return The wheel accelerations.
   */
  virtual WheelAccelerations ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations) const = 0;
};
}  // namespace wpi::math
