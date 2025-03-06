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
   * Returns a trigger that is true when the robot is in a particular operation mode.
   *
   * @param mode operating mode
   * @return A trigger that is true when the robot is in the provided mode.
   */
  public static Trigger opMode(int mode) {
    return new Trigger(() -> DriverStation.isOpMode(mode));
  }

  /**
   * Returns a trigger that is true when the robot is in a particular operation mode.
   *
   * @param mode operating mode
   * @return A trigger that is true when the robot is in the provided mode.
   */
  public static Trigger opMode(String mode) {
    return new Trigger(() -> DriverStation.isOpMode(mode));
  }

  /**
   * Returns a trigger that is true when the robot is disabled.
   *
   * @return A trigger that is true when the robot is disabled.
   */
  public static Trigger disabled() {
    return new Trigger(DriverStation::isDisabled);
  }
}
