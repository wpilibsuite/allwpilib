// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class VoltageUnitTest {
  @Test
  void testVoltsTimesAmps() {
    assertTrue(Units.Volts.mult(Units.Amps, "", "").equivalent(Units.Watts));
  }

  @Test
  void testMilliVoltsTimesMilliAmps() {
    // results in microwatts
    assertTrue(Units.Millivolts.mult(Units.Milliamps, "", "").equivalent(Units.Micro(Units.Watts)));
  }
}
