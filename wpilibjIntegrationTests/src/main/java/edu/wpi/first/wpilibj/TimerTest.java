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
  private static final long TIMER_TOLERANCE_MICROS = (long) (2.5 * 1000);
  private static final long TIMER_RUNTIME_MICROS = 5 * 1000000;

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void delayTest() {
    // Given
    long startTimeMicroS = RobotController.getFPGATimeMicroSeconds();

    // When
    Timer.delay(TIMER_RUNTIME_MICROS / 1000000);
    long endTimeMicroS = RobotController.getFPGATimeMicroSeconds();
    long differenceMicroS = endTimeMicroS - startTimeMicroS;

    // Then
    long offset = differenceMicroS - TIMER_RUNTIME_MICROS;
    assertEquals("Timer.delay ran " + offset + " microseconds too long", TIMER_RUNTIME_MICROS,
        differenceMicroS, TIMER_TOLERANCE_MICROS);
  }

}
