// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class CurrentTest {
  @Test
  void testAmpsTimesVolts() {
    Power combined = Units.Amps.times(Units.Volts, "Watt", "w");

    assertTrue(combined.equivalent(Units.Watts));
  }

  @Test
  void testMilliAmpsTimesMilliVolts() {
    // results in microwatts
    assertTrue(
        Units.Milliamps.times(Units.Millivolts, "Microwatt", "uW")
            .equivalent(Units.Milli(Units.Milliwatts)));
  }
}
