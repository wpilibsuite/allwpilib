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
import static edu.wpi.first.units.Units.Second;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class VelocityTest {
  @Test
  void testBaseUnit() {
    assertTrue(MetersPerSecond.equivalent(BaseUnits.Velocity));
    assertTrue(Meters.per(Second).equivalent(BaseUnits.Velocity));
  }

  @Test
  void testToAcceleration() {
    Velocity<Velocity<Distance>> metersPerSecondPerMillisecond = MetersPerSecond.per(Millisecond);

    assertEquals(1000, metersPerSecondPerMillisecond.of(1).in(MetersPerSecondPerSecond), 0);
    assertEquals(0, metersPerSecondPerMillisecond.of(0).in(MetersPerSecondPerSecond), 0);
  }

  @Test
  void testCache() {
    assertSame(
        FeetPerSecond, Feet.per(Second), "Feet.per(Second) should return a cached object instance");

    // completely arbitrary units chosen because they won't have already been cached
    var someDistance = new ExampleUnit(5);
    var someTime = new ExampleUnit(600);
    var firstInvocation = someDistance.per(someTime);
    var secondInvocation = someDistance.per(someTime);
    assertSame(
        firstInvocation,
        secondInvocation,
        firstInvocation + " was not the same object as " + secondInvocation);
  }

  @Test
  void testMult() {
    var baseUnit = new ExampleUnit(92);
    var vel = baseUnit.per(Millisecond);
    var mult = vel.mult(Second);
    assertEquals(92_000, mult.toBaseUnits(1), 1e-5);
  }
}
