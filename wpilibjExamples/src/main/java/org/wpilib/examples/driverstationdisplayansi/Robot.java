// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.driverstationdisplayansi;

import org.wpilib.driverstation.DriverStationDisplay;
import org.wpilib.framework.TimedRobot;
import org.wpilib.system.Timer;

/**
 * Demonstrates DriverStationDisplay raw ANSI mode. Static display text is written once, then a
 * value and its color are updated in place once per second.
 */
public class Robot extends TimedRobot {
  private static final int[] kColors = {
    196, 202, 208, 214, 220, 226, 118, 46, 48, 51,
    45, 39, 33, 27, 21, 57, 93, 129, 165, 201
  };

  private final Timer timer = new Timer();
  private int seconds;

  /** Called once at the beginning of the robot program. */
  public Robot() {
    DriverStationDisplay.setRawMode();
    timer.start();

    DriverStationDisplay.writeRawAnsiText(
        "\033[2J\033[H"
            + "DriverStationDisplay ANSI mode\n"
            + "This header and footer stay on screen.\n"
            + "Seconds elapsed:      \n"
            + "Only the number above is rewritten.");
    updateSecondsDisplay();
  }

  /** Called every robot loop. */
  @Override
  public void robotPeriodic() {
    if (timer.advanceIfElapsed(1.0)) {
      seconds++;
      updateSecondsDisplay();
    }
  }

  private void updateSecondsDisplay() {
    int color = kColors[seconds % kColors.length];
    DriverStationDisplay.writeRawAnsiText(
        String.format("\033[3;18H\033[38;5;%dm%5d\033[0m", color, seconds));
  }
}
