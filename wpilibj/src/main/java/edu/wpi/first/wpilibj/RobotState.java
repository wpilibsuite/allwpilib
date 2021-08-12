// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

@SuppressWarnings("MissingJavadocMethod")
public final class RobotState {
  public static boolean isDisabled() {
    return DriverStation.isDisabled();
  }

  public static boolean isEnabled() {
    return DriverStation.isEnabled();
  }

  public static boolean isEStopped() {
    return DriverStation.isEStopped();
  }

  @Deprecated
  public static boolean isOperatorControl() {
    return isTeleop();
  }

  public static boolean isTeleop() {
    return DriverStation.isTeleop();
  }

  public static boolean isAutonomous() {
    return DriverStation.isAutonomous();
  }

  public static boolean isTest() {
    return DriverStation.isTest();
  }

  private RobotState() {}
}
