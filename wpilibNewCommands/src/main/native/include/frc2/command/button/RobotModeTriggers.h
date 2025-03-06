// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "frc2/command/button/Trigger.h"

namespace frc2 {

/**
 * A class containing static Trigger factories for running callbacks when robot
 * mode changes.
 */
class RobotModeTriggers {
 public:
  RobotModeTriggers() = delete;

  /**
   * Returns a trigger that is true when the robot is in a particular operation
   * mode.
   *
   * @param mode operating mode
   * @return A trigger that is true when the robot is in the provided mode.
   */
  static Trigger OpMode(int mode);

  /**
   * Returns a trigger that is true when the robot is in a particular operation
   * mode.
   *
   * @param mode operating mode
   * @return A trigger that is true when the robot is in the provided mode.
   */
  static Trigger OpMode(std::string_view mode);

  /**
   * Returns a trigger that is true when the robot is disabled.
   *
   * @return A trigger that is true when the robot is disabled.
   */
  static Trigger Disabled();
};

}  // namespace frc2
