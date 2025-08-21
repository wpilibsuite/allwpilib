// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static edu.wpi.first.units.Units.RadiansPerSecondPerSecond;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class SwerveModuleAccelerationsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDefaultConstructor() {
    var moduleAccelerations = new SwerveModuleAccelerations();

    assertAll(
        () -> assertEquals(0.0, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(0.0, moduleAccelerations.angularAcceleration, kEpsilon));
  }

  @Test
  void testParameterizedConstructor() {
    var moduleAccelerations = new SwerveModuleAccelerations(2.5, 1.5);

    assertAll(
        () -> assertEquals(2.5, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(1.5, moduleAccelerations.angularAcceleration, kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var acceleration = MetersPerSecondPerSecond.of(3.0);
    var angularAcceleration = RadiansPerSecondPerSecond.of(2.0);
    var moduleAccelerations = new SwerveModuleAccelerations(acceleration, angularAcceleration);

    assertAll(
        () -> assertEquals(3.0, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(2.0, moduleAccelerations.angularAcceleration, kEpsilon));
  }

  @Test
  void testEquals() {
    var moduleAccelerations1 = new SwerveModuleAccelerations(2.0, 1.5);
    var moduleAccelerations2 = new SwerveModuleAccelerations(2.0, 1.5);
    var moduleAccelerations3 = new SwerveModuleAccelerations(2.1, 1.5);

    assertEquals(moduleAccelerations1, moduleAccelerations2);
    assertNotEquals(moduleAccelerations1, moduleAccelerations3);
  }

  @Test
  void testCompareTo() {
    var slower = new SwerveModuleAccelerations(1.0, 2.0);
    var faster = new SwerveModuleAccelerations(2.0, 1.0);

    assertTrue(slower.compareTo(faster) < 0);
    assertTrue(faster.compareTo(slower) > 0);
    assertEquals(0, slower.compareTo(slower));
  }

  @Test
  void testPlus() {
    final var left = new SwerveModuleAccelerations(1.0, 0.5);
    final var right = new SwerveModuleAccelerations(2.0, 1.5);

    final var moduleAccelerations = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, moduleAccelerations.acceleration),
        () -> assertEquals(2.0, moduleAccelerations.angularAcceleration));
  }

  @Test
  void testMinus() {
    final var left = new SwerveModuleAccelerations(3.0, 2.5);
    final var right = new SwerveModuleAccelerations(1.0, 0.5);

    final var moduleAccelerations = left.minus(right);

    assertAll(
        () -> assertEquals(2.0, moduleAccelerations.acceleration),
        () -> assertEquals(2.0, moduleAccelerations.angularAcceleration));
  }

  @Test
  void testUnaryMinus() {
    final var moduleAccelerations = new SwerveModuleAccelerations(1.5, -2.0).unaryMinus();

    assertAll(
        () -> assertEquals(-1.5, moduleAccelerations.acceleration),
        () -> assertEquals(2.0, moduleAccelerations.angularAcceleration));
  }

  @Test
  void testMultiplication() {
    final var moduleAccelerations = new SwerveModuleAccelerations(2.0, 1.5).times(2.0);

    assertAll(
        () -> assertEquals(4.0, moduleAccelerations.acceleration),
        () -> assertEquals(3.0, moduleAccelerations.angularAcceleration));
  }

  @Test
  void testDivision() {
    final var moduleAccelerations = new SwerveModuleAccelerations(4.0, 2.0).div(2.0);

    assertAll(
        () -> assertEquals(2.0, moduleAccelerations.acceleration),
        () -> assertEquals(1.0, moduleAccelerations.angularAcceleration));
  }

  @Test
  void testInterpolate() {
    final var start = new SwerveModuleAccelerations(1.0, 2.0);
    final var end = new SwerveModuleAccelerations(5.0, 6.0);

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.acceleration, kEpsilon),
        () -> assertEquals(2.0, atStart.angularAcceleration, kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.acceleration, kEpsilon),
        () -> assertEquals(6.0, atEnd.angularAcceleration, kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.acceleration, kEpsilon),
        () -> assertEquals(4.0, atMidpoint.angularAcceleration, kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.acceleration, kEpsilon),
        () -> assertEquals(3.0, atQuarter.angularAcceleration, kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.acceleration, kEpsilon),
        () -> assertEquals(2.0, belowRange.angularAcceleration, kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.acceleration, kEpsilon),
        () -> assertEquals(6.0, aboveRange.angularAcceleration, kEpsilon));
  }
}
