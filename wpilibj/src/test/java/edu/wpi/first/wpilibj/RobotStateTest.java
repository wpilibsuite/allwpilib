/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.Timeout;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.sim.DriverStationSim;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class RobotStateTest extends UtilityClassTest<RobotState> {
  RobotStateTest() {
    super(RobotState.class);
  }

  @Test
  void isDisabledTest(@SimDs DriverStationSim ds) {
    HAL.isNewControlData();
    ds.setEnabled(false);
    ds.notifyNewData();

    ds.waitForCachedControlData();

    assertTrue(RobotState.isDisabled());
  }

  @Test
  void isNotDisabledTest(@SimDs DriverStationSim ds) {
    HAL.isNewControlData();
    ds.setEnabled(true);
    ds.notifyNewData();

    ds.waitForCachedControlData();

    assertFalse(RobotState.isDisabled());
  }
}
