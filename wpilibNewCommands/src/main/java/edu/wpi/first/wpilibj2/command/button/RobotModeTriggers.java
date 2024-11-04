// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.DriverStation;

/**
 * A class containing static {@link Trigger} factories for running callbacks when the robot mode
 * changes.
 */
public final class RobotModeTriggers {
  // Utility class
  private RobotModeTriggers() {}

  /**
   * A trigger that is true when the robot is enabled in autonomous mode.
   */
  public static Trigger autonomous = new Trigger(DriverStation::isAutonomousEnabled);

  /**
   * A trigger that is true when the robot is enabled in teleop mode.
   */
  public static Trigger teleop = new Trigger(DriverStation::isTeleopEnabled);

  /**
   * A trigger that is true when the robot is disabled.
   */
  public static Trigger disabled = new Trigger(DriverStation::isDisabled);

  /**
   * A trigger that is true when the robot is enabled in test mode.
   */
  public static Trigger test = new Trigger(DriverStation::isTestEnabled);
}
