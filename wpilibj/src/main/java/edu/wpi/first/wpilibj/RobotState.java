/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

@SuppressWarnings("JavadocMethod")
public final class RobotState {
  private RobotState() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  public static boolean isDisabled() {
    return DriverStation.getInstance().isDisabled();
  }

  public static boolean isEnabled() {
    return DriverStation.getInstance().isEnabled();
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
}
