// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Twist2d.h"
#include "frc/kinematics/ChassisSpeeds.h"

namespace frc {
/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual wheel speeds.
 *
 * Inverse kinematics converts a desired chassis speed into wheel speeds whereas
 * forward kinematics converts wheel speeds into chassis speed.
 */
template <typename T>
class WPILIB_DLLEXPORT Kinematics {
 public:
  /**
   * Performs forward kinematics to return the resulting Twist2d from the given
   * wheel deltas. This method is often used for odometry -- determining the
   * robot's position on the field using changes in the distance driven by each
   * wheel on the robot.
   *
   * @param wheelDeltas The distances driven by each wheel.
   *
   * @return The resulting Twist2d in the robot's movement.
   */
  virtual Twist2d ToTwist2d(const T& wheelDeltas) const = 0;
};
}  // namespace frc
