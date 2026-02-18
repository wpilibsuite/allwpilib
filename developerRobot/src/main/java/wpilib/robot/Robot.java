// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package wpilib.robot;

import org.wpilib.driverstation.DefaultDriverStation;
import org.wpilib.driverstation.DriverStationInstance;
import org.wpilib.framework.OpModeRobot;

@DriverStationInstance(DefaultDriverStation.class)
public class Robot extends OpModeRobot {
  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot(DefaultDriverStation defaultDs) {}
}
