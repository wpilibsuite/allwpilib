// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

@SuppressWarnings("MissingJavadocMethod")
public final class RobotState {
  public static boolean isDisabled() {
    return DriverStation.getInstance().isDisabled();
  }

  public static boolean isEnabled() {
    return DriverStation.getInstance().isEnabled();
  }

  public static boolean isEStopped() {
    return DriverStation.getInstance().isEStopped();
  }

  public static boolean isOperatorControl() {
    return DriverStation.getInstance().isOperatorControl();
  }

  public static boolean isAutonomous() {
    return DriverStation.getInstance().isAutonomous();
  }

  public static boolean isTest() {
    return DriverStation.getInstance().isTest();
  }

  private RobotState() {}
}
