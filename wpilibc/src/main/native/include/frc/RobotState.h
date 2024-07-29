// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/**
 * Robot state utility functions.
 */
class RobotState {
 public:
  RobotState() = delete;

  /**
   * Returns true if the robot is disabled.
   *
   * @return True if the robot is disabled.
   */
  static bool IsDisabled();

  /**
   * Returns true if the robot is enabled.
   *
   * @return True if the robot is enabled.
   */
  static bool IsEnabled();

  /**
   * Returns true if the robot is E-stopped.
   *
   * @return True if the robot is E-stopped.
   */
  static bool IsEStopped();

  /**
   * Returns true if the robot is in teleop mode.
   *
   * @return True if the robot is in teleop mode.
   */
  static bool IsTeleop();

  /**
   * Returns true if the robot is in autonomous mode.
   *
   * @return True if the robot is in autonomous mode.
   */
  static bool IsAutonomous();

  /**
   * Returns true if the robot is in test mode.
   *
   * @return True if the robot is in test mode.
   */
  static bool IsTest();
};

}  // namespace frc
