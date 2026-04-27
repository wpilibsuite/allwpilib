// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cmath>
#include <stdexcept>
#include <type_traits>

#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Kinematics.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelAccelerations.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelPositions.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelVelocities.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Helper class that converts follower wheel velocities (vx1, vx2, vy) into
 * chassis velocities (dx, dy, dtheta) and vice versa for a triple follower
 * wheel odometry setup.
 *
 * Inverse kinematics converts a desired chassis velocity into wheel velocities,
 * whereas forward kinematics converts wheel velocities into a chassis velocity.
 *
 * This class is primarily used for odometry -- determining the position of the
 * robot on the field using forward kinematics with wheel encoders and a gyro.
 */
class WPILIB_DLLEXPORT TripleFollowerWheelKinematics
    : public Kinematics<TripleFollowerWheelPositions,
                        TripleFollowerWheelVelocities,
                        TripleFollowerWheelAccelerations> {
 public:
  /**
   * Constructs a TripleFollowerWheelKinematics object.
   *
   * @param xWheel1YPos The distance from the center of the robot to the 1st
   *     forward-facing wheel along the y-axis.
   * @param xWheel2YPos The distance from the center of the robot to the 2nd
   *     forward-facing wheel along the y-axis.
   * @param yWheelXPos The distance from the center of the robot to the
   *     left-facing wheel along the x-axis.
   */
  constexpr TripleFollowerWheelKinematics(wpi::units::meter_t xWheel1YPos,
                                          wpi::units::meter_t xWheel2YPos,
                                          wpi::units::meter_t yWheelXPos)
      : xWheel1YPos(xWheel1YPos),
        xWheel2YPos(xWheel2YPos),
        yWheelXPos(yWheelXPos) {
    if (!std::is_constant_evaluated()) {
      auto Sign = [](double value) {
        return (value > 0.0) - (value < 0.0);
      };

      if (Sign(xWheel1YPos.value()) == Sign(xWheel2YPos.value())) {
        throw std::invalid_argument(
            "The x axis wheels must be on opposite sides of the robot");
      }

      wpi::math::MathSharedStore::ReportUsage("TripleFollowerWheelKinematics",
                                              "");
    }
  }

  /**
   * Returns a chassis velocity from follower wheel velocities using forward
   * kinematics.
   *
   * @param wheelVelocities The follower wheel velocities.
   * @return The chassis velocity.
   */
  constexpr ChassisVelocities ToChassisVelocities(
      const TripleFollowerWheelVelocities& wheelVelocities) const override {
    auto omega =
        (wheelVelocities.vx1 - wheelVelocities.vx2) /
        (xWheel2YPos - xWheel1YPos) * 1_rad;
    auto vx = (wheelVelocities.vx1 + wheelVelocities.vx2) / 2.0;
    auto vy = wheelVelocities.y - yWheelXPos * omega / 1_rad;
    return {vx, vy, omega};
  }

  /**
   * Returns follower wheel velocities from a chassis velocity using inverse
   * kinematics.
   *
   * @param chassisVelocities The chassis velocity.
   * @return The follower wheel velocities.
   */
  constexpr TripleFollowerWheelVelocities ToWheelVelocities(
      const ChassisVelocities& chassisVelocities) const override {
    return {chassisVelocities.vx -
                xWheel1YPos * chassisVelocities.omega / 1_rad,
            chassisVelocities.vx -
                xWheel2YPos * chassisVelocities.omega / 1_rad,
            chassisVelocities.vy +
                yWheelXPos * chassisVelocities.omega / 1_rad};
  }

  /**
   * Returns a chassis acceleration from follower wheel accelerations using
   * forward kinematics.
   *
   * @param wheelAccelerations The follower wheel accelerations.
   * @return The chassis accelerations.
   */
  constexpr ChassisAccelerations ToChassisAccelerations(
      const TripleFollowerWheelAccelerations& wheelAccelerations)
      const override {
    auto alpha =
        (wheelAccelerations.ax1 - wheelAccelerations.ax2) /
        (xWheel2YPos - xWheel1YPos) * 1_rad;
    auto ax = (wheelAccelerations.ax1 + wheelAccelerations.ax2) / 2.0;
    auto ay = wheelAccelerations.ay - yWheelXPos * alpha / 1_rad;
    return {ax, ay, alpha};
  }

  /**
   * Returns follower wheel accelerations from a chassis acceleration using
   * inverse kinematics.
   *
   * @param chassisAccelerations The chassis accelerations.
   * @return The follower wheel accelerations.
   */
  constexpr TripleFollowerWheelAccelerations ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations) const override {
    return {chassisAccelerations.ax -
                xWheel1YPos * chassisAccelerations.alpha / 1_rad,
            chassisAccelerations.ax -
                xWheel2YPos * chassisAccelerations.alpha / 1_rad,
            chassisAccelerations.ay +
                yWheelXPos * chassisAccelerations.alpha / 1_rad};
  }

  /**
   * Returns a twist from follower wheel distance deltas using forward
   * kinematics.
   *
   * @param start The starting distances driven by the wheels.
   * @param end The ending distances driven by the wheels.
   * @return The resulting Twist2d.
   */
  constexpr Twist2d ToTwist2d(const TripleFollowerWheelPositions& start,
                              const TripleFollowerWheelPositions& end) const
      override {
    auto deltaXWheel1 = end.x1 - start.x1;
    auto deltaXWheel2 = end.x2 - start.x2;
    auto deltaYWheel = end.y - start.y;

    auto deltaTheta = (deltaXWheel1 - deltaXWheel2) /
                          (xWheel2YPos - xWheel1YPos) *
                      1_rad;
    auto deltaX = (deltaXWheel1 + deltaXWheel2) / 2.0;
    auto deltaY = deltaYWheel - yWheelXPos * deltaTheta / 1_rad;
    return {deltaX, deltaY, deltaTheta};
  }

  constexpr TripleFollowerWheelPositions Interpolate(
      const TripleFollowerWheelPositions& start,
      const TripleFollowerWheelPositions& end, double t) const override {
    return start.Interpolate(end, t);
  }

  /// The distance from the center of the robot to the 1st forward-facing wheel
  /// along the y-axis.
  wpi::units::meter_t xWheel1YPos;

  /// The distance from the center of the robot to the 2nd forward-facing wheel
  /// along the y-axis.
  wpi::units::meter_t xWheel2YPos;

  /// The distance from the center of the robot to the left-facing wheel along
  /// the x-axis.
  wpi::units::meter_t yWheelXPos;
};
}  // namespace wpi::math
