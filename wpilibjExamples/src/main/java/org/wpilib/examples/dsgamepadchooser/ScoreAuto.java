// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.dsgamepadchooser;

import java.util.List;
import org.wpilib.driverstation.DriverStationDisplay;
import org.wpilib.driverstation.DriverStationDisplay.Mode;
import org.wpilib.driverstation.DSGamepadChooser;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.PeriodicOpMode;

/** An autonomous opmode with chooser scoring parameters. */
@Autonomous(name = "Score Auto")
public class ScoreAuto extends PeriodicOpMode {
  private final DSGamepadChooser chooser = new DSGamepadChooser(0);
  private String target = "High";
  private int delay;
  private double speed = 0.25;

  /** Creates a Score Auto opmode. */
  public ScoreAuto() {
    DriverStationDisplay.setMode(Mode.Line);
    chooser.addOptions("Target", List.of("High", "Mid", "Low"));
    chooser.addIntegerOptions("Delay", 0, 5, 1);
    chooser.addDoubleOptions("Speed", 0.25, 1.0, 0.25);
  }

  /** Called periodically while this opmode is selected and the robot is disabled. */
  @Override
  public void disabledPeriodic() {
    chooser.update();
    DriverStationDisplay.updateLines();
  }

  /** Called once when the robot transitions from disabled to enabled. */
  @Override
  public void start() {
    target = chooser.getSelected("Target");
    delay = chooser.getSelectedInteger("Delay");
    speed = chooser.getSelectedDouble("Speed");
  }

  /** Called periodically while this opmode is enabled. */
  @Override
  public void periodic() {
    DriverStationDisplay.addData("Selected Auto", "Score");
    DriverStationDisplay.addData("Target", target);
    DriverStationDisplay.addData("Delay", delay);
    DriverStationDisplay.addData("Speed", speed);
    DriverStationDisplay.updateLines();
  }
}
