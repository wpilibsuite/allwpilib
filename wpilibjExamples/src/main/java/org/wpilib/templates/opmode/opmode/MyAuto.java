// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.opmode.opmode;

import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.templates.opmode.Robot;

@Autonomous(name = "My Auto", group = "Group 1")
public class MyAuto extends PeriodicOpMode {
  private final Robot m_robot;

  /** The Robot instance is passed into the opmode via the constructor. */
  public MyAuto(Robot robot) {
    super(robot);
    m_robot = robot;
    /*
     * Can call super(period) to set a different periodic time interval.
     *
     * Additional periodic methods may be configured with addPeriodic().
     */
  }

  @Override
  public void periodic() {
    // Put custom auto code here
  }
}
