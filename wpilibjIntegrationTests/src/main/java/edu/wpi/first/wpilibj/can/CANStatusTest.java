// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.can;

import edu.wpi.first.hal.can.CANJNI;
import edu.wpi.first.hal.can.CANStatus;
import org.junit.Test;

public class CANStatusTest {
  @Test
  public void canStatusGetDoesntThrow() {
    CANStatus status = new CANStatus();
    CANJNI.getCANStatus(status);
    // Nothing we can assert, so just make sure it didn't throw.
  }
}
