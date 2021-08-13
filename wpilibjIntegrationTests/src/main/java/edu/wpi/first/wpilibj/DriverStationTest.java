// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.jupiter.api.Test;

public class DriverStationTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(TimerTest.class.getName());
  private static final double TIMER_TOLERANCE = 0.2;
  private static final long TIMER_RUNTIME = 1000000; // 1 second

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void waitForDataTest() {
    long startTime = RobotController.getFPGATime();

    // Wait for data 50 times
    for (int i = 0; i < 50; i++) {
      DriverStation.waitForData();
    }
    long endTime = RobotController.getFPGATime();
    long difference = endTime - startTime;

    assertEquals(
        TIMER_RUNTIME,
        difference,
        TIMER_TOLERANCE * TIMER_RUNTIME,
        "DriverStation waitForData did not wait long enough");
  }
}
