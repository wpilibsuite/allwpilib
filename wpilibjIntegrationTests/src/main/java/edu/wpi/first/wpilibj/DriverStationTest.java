/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.Test;

import java.util.logging.Logger;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;


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
    long startTime = Utility.getFPGATime();

    // Wait for data 50 times
    for (int i = 0; i < 50; i++) {
      DriverStation.getInstance().waitForData();
    }
    long endTime = Utility.getFPGATime();
    long difference = endTime - startTime;

    assertEquals("DriverStation waitForData did not wait long enough", TIMER_RUNTIME, difference,
        TIMER_TOLERANCE * TIMER_RUNTIME);
  }

}
