// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

public final class RobotState {
  /**
   * Returns true if the robot is disabled.
   *
   * @return True if the robot is disabled.
   */
  public static boolean isDisabled() {
    return DriverStation.isDisabled();
  }

  /**
   * Returns true if the robot is enabled.
   *
   * @return True if the robot is enabled.
   */
  public static boolean isEnabled() {
    return DriverStation.isEnabled();
  }

  /**
   * Returns true if the robot is E-stopped.
   *
   * @return True if the robot is E-stopped.
   */
  public static boolean isEStopped() {
    return DriverStation.isEStopped();
  }

  /**
   * Returns true if the robot is in teleop mode.
   *
   * @return True if the robot is in teleop mode.
   */
  public static boolean isTeleop() {
    return DriverStation.isTeleop();
  }

  /**
   * Returns true if the robot is in autonomous mode.
   *
   * @return True if the robot is in autonomous mode.
   */
  public static boolean isAutonomous() {
    return DriverStation.isAutonomous();
  }

  /**
   * Returns true if the robot is in test mode.
   *
   * @return True if the robot is in test mode.
   */
  public static boolean isTest() {
    return DriverStation.isTest();
  }

  private RobotState() {}
}
