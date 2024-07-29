// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class VoltageTest {
  @Test
  void testVoltsTimesAmps() {
    assertTrue(Units.Volts.times(Units.Amps, "", "").equivalent(Units.Watts));
  }

  @Test
  void testMilliVoltsTimesMilliAmps() {
    // results in microwatts
    assertTrue(
        Units.Millivolts.times(Units.Milliamps, "", "").equivalent(Units.Milli(Units.Milliwatts)));
  }
}
