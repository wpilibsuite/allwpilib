// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Feet;
import static edu.wpi.first.units.Units.FeetPerSecond;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.MetersPerSecond;
import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.Millisecond;
import static edu.wpi.first.units.Units.Minute;
import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class VelocityUnitTest {
  @Test
  void testBaseUnit() {
    assertTrue(MetersPerSecond.equivalent(MetersPerSecond));
    assertTrue(Meters.per(Second).equivalent(MetersPerSecond));
  }

  @Test
  void testToAcceleration() {
    LinearAccelerationUnit metersPerSecondPerMillisecond = MetersPerSecond.per(Millisecond);

    assertEquals(1000, metersPerSecondPerMillisecond.of(1).in(MetersPerSecondPerSecond), 0);
    assertEquals(0, metersPerSecondPerMillisecond.of(0).in(MetersPerSecondPerSecond), 0);
  }

  @Test
  void testCache() {
    assertSame(
        FeetPerSecond, Feet.per(Second), "Feet.per(Second) should return a cached object instance");

    // completely arbitrary units chosen because they won't have already been cached
    var someDistance = new AngleUnit(Radians, 123, "a", "a");
    var someTime = new TimeUnit(Seconds, 123, "t", "t");
    var firstInvocation = someDistance.per(someTime);
    var secondInvocation = someDistance.per(someTime);
    assertSame(
        firstInvocation,
        secondInvocation,
        firstInvocation + " was not the same object as " + secondInvocation);
  }

  @Test
  void testMult() {
    // 92 per millisecond => 92,000 per second (base unit equivalent) => 5,520,000 per minute
    var baseUnit = new ExampleUnit(92);
    var vel = VelocityUnit.combine(baseUnit, Millisecond);
    var mult = vel.mult(Minute);
    assertEquals(1 / 92000.0, mult.fromBaseUnits(1), 1e-5);
    assertEquals(5_520_000, mult.toBaseUnits(1), 1e-5);
  }
}
