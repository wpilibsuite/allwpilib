// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.gamepadselectable;

import java.util.List;
import org.wpilib.driverstation.DriverStationDisplay;
import org.wpilib.driverstation.DriverStationDisplay.Mode;
import org.wpilib.driverstation.GamepadSelectable;
import org.wpilib.opmode.Autonomous;
import org.wpilib.opmode.PeriodicOpMode;

/** An autonomous opmode with selectable balance parameters. */
@Autonomous(name = "Balance Auto")
public class BalanceAuto extends PeriodicOpMode {
  private final GamepadSelectable selectable = new GamepadSelectable(0);
  private String approach = "Center";
  private int attempts = 1;
  private double turnScale = 0.5;

  /** Creates a Balance Auto opmode. */
  public BalanceAuto() {
    DriverStationDisplay.setMode(Mode.Line);
    selectable.addOptions("Approach", List.of("Left", "Center", "Right"));
    selectable.addIntegerOptions("Attempts", 1, 3, 1);
    selectable.addDoubleOptions("Turn Scale", 0.2, 1.0, 0.2);
  }

  /** Called periodically while this opmode is selected and the robot is disabled. */
  @Override
  public void disabledPeriodic() {
    selectable.update();
    DriverStationDisplay.updateLines();
  }

  /** Called once when the robot transitions from disabled to enabled. */
  @Override
  public void start() {
    approach = selectable.getSelected("Approach");
    attempts = selectable.getSelectedInteger("Attempts");
    turnScale = selectable.getSelectedDouble("Turn Scale");
  }

  /** Called periodically while this opmode is enabled. */
  @Override
  public void periodic() {
    DriverStationDisplay.addData("Selected Auto", "Balance");
    DriverStationDisplay.addData("Approach", approach);
    DriverStationDisplay.addData("Attempts", attempts);
    DriverStationDisplay.addData("Turn Scale", turnScale);
    DriverStationDisplay.updateLines();
  }
}
