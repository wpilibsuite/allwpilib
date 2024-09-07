// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class CurrentUnitTest {
  @Test
  void testAmpsTimesVolts() {
    PowerUnit combined = Units.Amps.mult(Units.Volts, "Watt", "w");

    assertTrue(combined.equivalent(Units.Watts));
  }

  @Test
  void testMilliAmpsTimesMilliVolts() {
    // results in microwatts
    PowerUnit times = Units.Milliamps.mult(Units.Millivolts, "Microwatt", "uW");
    PowerUnit millimilli = Units.Milli(Units.Milliwatts);

    assertEquals(1, times.convertFrom(1e-6, Units.Watts));
    assertEquals(1, millimilli.convertFrom(1e-6, Units.Watts));
    assertTrue(times.equivalent(millimilli));
  }
}
