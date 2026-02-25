// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import org.wpilib.driverstation.DriverStation;

/**
 * A class containing static {@link Trigger} factories for running callbacks when the robot mode
 * changes.
 */
public final class RobotModeTriggers {
  // Utility class
  private RobotModeTriggers() {}

  /**
   * Returns a trigger that is true when the robot is enabled in autonomous mode.
   *
   * @return A trigger that is true when the robot is enabled in autonomous mode.
   */
  public static Trigger autonomous() {
    return new Trigger(DriverStation::isAutonomousEnabled);
  }

  /**
   * Returns a trigger that is true when the robot is enabled in teleop mode.
   *
   * @return A trigger that is true when the robot is enabled in teleop mode.
   */
  public static Trigger teleop() {
    return new Trigger(DriverStation::isTeleopEnabled);
  }

  /**
   * Returns a trigger that is true when the robot is disabled.
   *
   * @return A trigger that is true when the robot is disabled.
   */
  public static Trigger disabled() {
    return new Trigger(DriverStation::isDisabled);
  }

  /**
   * Returns a trigger that is true when the robot is enabled in test mode.
   *
   * @return A trigger that is true when the robot is enabled in test mode.
   */
  public static Trigger test() {
    return new Trigger(DriverStation::isTestEnabled);
  }
}
