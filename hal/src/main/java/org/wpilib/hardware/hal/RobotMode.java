// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Robot mode. Note this does not indicate enabled state. */
public enum RobotMode {
  /** Unknown. */
  UNKNOWN(0),
  /** Autonomous. */
  AUTONOMOUS(1),
  /** Teleoperated. */
  TELEOPERATED(2),
  /** Test. */
  TEST(3);

  private final int value;

  RobotMode(int value) {
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

  /** Gets a mode from an integer value. */
  public static RobotMode fromInt(int value) {
    return switch (value) {
      case 1 -> AUTONOMOUS;
      case 2 -> TELEOPERATED;
      case 3 -> TEST;
      default -> UNKNOWN;
    };
  }
}
