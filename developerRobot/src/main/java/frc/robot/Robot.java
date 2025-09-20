// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot;

import edu.wpi.first.wpilibj.OpModeRobot;

public class Robot extends OpModeRobot {
  private int m_count;

  /**
   * This function is run when the robot is first started up and should be used for any
   * initialization code.
   */
  public Robot() {
    System.out.println("Robot constructed");
  }

  /**
   * This function is called exactly once when the DS first connects.
   */
  public void driverStationConnected() {
    System.out.println("driver station connected");
  }

  /**
   * This function is called periodically anytime when no opmode is selected, including when the
   * Driver Station is disconnected.
   */
  @Override
  public void nonePeriodic() {
    if (m_count++ >= 20) {
      System.out.println("no opmode selected");
      m_count = 0;
    }
  }
}
