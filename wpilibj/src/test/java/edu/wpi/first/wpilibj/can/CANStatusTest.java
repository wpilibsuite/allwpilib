/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.can;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.can.CANJNI;
import edu.wpi.first.hal.can.CANStatus;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

class CANStatusTest {
  @Test
  void canStatusGetDoesntThrow() {
    HAL.initialize(500, 0);
    CANStatus status = new CANStatus();
    assertDoesNotThrow(() -> CANJNI.GetCANStatus(status));
  }
}
