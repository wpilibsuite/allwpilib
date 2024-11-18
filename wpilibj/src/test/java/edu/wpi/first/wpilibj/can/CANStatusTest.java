// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.can;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

import org.wpilib.hal.HAL;
import org.wpilib.hal.can.CANJNI;
import org.wpilib.hal.can.CANStatus;
import org.junit.jupiter.api.Test;

class CANStatusTest {
  @Test
  void canStatusGetDoesntThrow() {
    HAL.initialize(500, 0);
    CANStatus status = new CANStatus();
    assertDoesNotThrow(() -> CANJNI.getCANStatus(status));
  }
}
