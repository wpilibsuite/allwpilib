/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.logging.Logger;

import org.junit.Test;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;


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
    long startTime = RobotController.getFPGATime();

    // When
    Timer.delay(TIMER_RUNTIME / 1000000);
    long endTime = RobotController.getFPGATime();
    long difference = endTime - startTime;

    // Then
    long offset = difference - TIMER_RUNTIME;
    assertEquals("Timer.delay ran " + offset + " microseconds too long", TIMER_RUNTIME, difference,
        TIMER_TOLERANCE);
  }

}
