// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class DistanceUnitTest {
  @Test
  void testBaseUnitDistancePerTime() {
    LinearVelocityUnit anonBaseUnit =
        new DistanceUnit(null, 1, "D", "d").per(new TimeUnit(null, 1, "T", "t"));

    assertTrue(Units.MetersPerSecond.equivalent(anonBaseUnit));
  }

  @Test
  void testFeetPerSecond() {
    LinearVelocityUnit feetPerMillisecond = Units.Feet.per(Units.Milliseconds);

    // one foot per millisecond
    // = (1 / 3.28084) meters per (1 / 1000) seconds
    // = (1000 / 3.28084) meters per second
    double asBaseMeasure = feetPerMillisecond.of(1).in(Units.MetersPerSecond);
    assertEquals(1000 / 3.28084, asBaseMeasure, 1e-3);

    // one meter per second = 1 mm per millisecond = 0.00328084 feet per millisecond
    double asContrivedMeasure = Units.MetersPerSecond.of(1).in(feetPerMillisecond);
    assertEquals(3.28084 / 1000, asContrivedMeasure, 1e-8);
  }
}
