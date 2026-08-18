// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.driverstationdisplaylines;

import org.wpilib.driverstation.DriverStationDisplay;
import org.wpilib.driverstation.DriverStationDisplay.Mode;
import org.wpilib.opmode.PeriodicOpMode;
import org.wpilib.opmode.Teleop;
import org.wpilib.system.Timer;

@Teleop(name = "Default Teleop")
public class DefaultTeleop extends PeriodicOpMode {
  private final Timer timer = new Timer();
  private int loopCount;

  /** Called once when the robot is enabled. */
  @Override
  public void start() {
    DriverStationDisplay.setMode(Mode.Line);
    timer.restart();
    loopCount = 0;
  }

  /** Called periodically while the robot is enabled. */
  @Override
  public void periodic() {
    DriverStationDisplay.addLine("DriverStationDisplay line mode");
    DriverStationDisplay.addData("Runtime", "%.1f seconds", timer.get());
    DriverStationDisplay.addData("Loop Count", "%d", loopCount++);
    DriverStationDisplay.updateLines();
  }
}
