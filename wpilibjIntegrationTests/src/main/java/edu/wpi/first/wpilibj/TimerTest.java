// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.Assert.assertEquals;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;
import java.util.logging.Logger;
import org.junit.Test;

public class TimerTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(TimerTest.class.getName());
  private static final long TIMER_TOLERANCE = (long) (2.5 * 1000);
  private static final long TIMER_RUNTIME = 5 * 1000000;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void delayTest() {
    // Given
    long startTime = RobotController.getTime();

    // When
    Timer.delay(TIMER_RUNTIME / 1000000);
    long endTime = RobotController.getTime();
    long difference = endTime - startTime;

    // Then
    long offset = difference - TIMER_RUNTIME;
    assertEquals(
        "Timer.delay ran " + offset + " microseconds too long",
        TIMER_RUNTIME,
        difference,
        TIMER_TOLERANCE);
  }
}
