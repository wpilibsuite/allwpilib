// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/button/Trigger.hpp"

namespace wpi::cmd {

/**
 * A class containing static Trigger factories for running callbacks when robot
 * mode changes.
 */
class RobotModeTriggers {
 public:
  RobotModeTriggers() = delete;

  /**
   * Returns a trigger that is true when the robot is enabled in autonomous
   * mode.
   *
   * @return A trigger that is true when the robot is enabled in autonomous
   * mode.
   */
  static Trigger Autonomous();

  /**
   * Returns a trigger that is true when the robot is enabled in teleop mode.
   *
   * @return A trigger that is true when the robot is enabled in teleop mode.
   */
  static Trigger Teleop();

  /**
   * Returns a trigger that is true when the robot is disabled.
   *
   * @return A trigger that is true when the robot is disabled.
   */
  static Trigger Disabled();

  /**
   * Returns a trigger that is true when the robot is enabled in test mode.
   *
   * @return A trigger that is true when the robot is enabled in test mode.
   */
  static Trigger Test();
};

}  // namespace wpi::cmd
