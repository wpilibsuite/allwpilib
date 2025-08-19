// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Robot mode. Note this does not indicate enabled state. */
public enum RobotMode {
  /** Unknown. */
  Unknown(0),
  /** Autonomous. */
  Autonomous(1),
  /** Teleoperated. */
  Teleoperated(2),
  /** Test. */
  Test(3);

  private final int value;

  private RobotMode(int value) {
    this.value = value;
  }

  /**
   * Gets the integer value for the mode.
   *
   * @return value
   */
  public int getValue() {
    return value;
  }

  /**
   * Gets a mode from an integer value.
   */
  public static RobotMode fromInt(int value) {
    return switch (value) {
      case 1 -> Autonomous;
      case 2 -> Teleoperated;
      case 3 -> Test;
      default -> Unknown;
    };
  }
}
