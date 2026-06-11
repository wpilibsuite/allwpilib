// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.driverstationdisplaylines;

import org.wpilib.driverstation.DriverStationDisplay;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.Timer;

/**
 * Demonstrates DriverStationDisplay line mode. In line mode, all lines are added each loop and
 * updateLines() sends the pending set to the Driver Station display.
 */
public class Robot extends TimedRobot {
  private final Timer timer = new Timer();
  private int loopCount;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    DriverStationDisplay.setLineMode();
    timer.start();
  }

  /** Called every robot loop. */
  @Override
  public void robotPeriodic() {
    DriverStationDisplay.addLine("DriverStationDisplay line mode");
    DriverStationDisplay.addData("Runtime", "%.1f seconds", timer.get());
    DriverStationDisplay.addData("Loop Count", "%d", loopCount++);
    DriverStationDisplay.updateLines();
  }
}
