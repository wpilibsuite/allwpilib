// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.can;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.can.CANJNI;
import org.wpilib.hardware.hal.can.CANStatus;

class CANStatusTest {
  @Test
  void canStatusGetDoesntThrow() {
    HAL.initialize(500, 0);
    CANStatus status = new CANStatus();
    assertDoesNotThrow(() -> CANJNI.getCANStatus(0, status));
  }
}
