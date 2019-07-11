/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.sim.DriverStationSim;

class RobotStateTest extends UtilityClassTest {
  RobotStateTest() {
    super(RobotState.class);
  }

  @Test
  void isDisabledTest(@SimDs DriverStationSim ds) {
    ds.setEnabled(false);

    assertTrue(RobotState.isDisabled());
  }

  @Test
  void isNotDisabledTest(@SimDs DriverStationSim ds) throws InterruptedException {
    ds.setEnabled(true);

    Thread.sleep(55);

    assertFalse(RobotState.isDisabled());
  }
}
