/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.logging.Logger;

import org.junit.Test;

import edu.wpi.first.wpilibj.test.AbstractComsSetup;

import static org.junit.Assert.assertEquals;


public class DriverStationTest extends AbstractComsSetup {
  private static final Logger logger = Logger.getLogger(TimerTest.class.getName());
  private static final double TIMER_TOLERANCE = 0.2;
  private static final long TIMER_RUNTIME_MICROS = 1000000; // 1 second

  @Override
  protected Logger getClassLogger() {
    return logger;
  }

  @Test
  public void waitForDataTest() {
    long startTimeMicroS = RobotController.getFPGATimeMicroSeconds();

    // Wait for data 50 times
    for (int i = 0; i < 50; i++) {
      DriverStation.getInstance().waitForData();
    }
    long endTimeMicroS = RobotController.getFPGATimeMicroSeconds();
    long differenceMicroS = endTimeMicroS - startTimeMicroS;

    assertEquals("DriverStation waitForData did not wait long enough", TIMER_RUNTIME_MICROS,
        differenceMicroS, TIMER_TOLERANCE * TIMER_RUNTIME_MICROS);
  }

}
