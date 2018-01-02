/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.Test;

import edu.wpi.first.wpilibj.hal.HAL;

public class CANStatusTest {
  @Test
  public void canStatusGetDoesntThrow() {
    HAL.initialize(500, 0);
    CANStatus status = new CANStatus();
    CANJNI.GetCANStatus(status);
    // Nothing we can assert, so just make sure it didn't throw.
  }
}
