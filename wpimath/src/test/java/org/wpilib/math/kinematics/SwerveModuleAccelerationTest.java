// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.MetersPerSecondPerSecond;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;

class SwerveModuleAccelerationTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDefaultConstructor() {
    var moduleAccelerations = new SwerveModuleAcceleration();

    assertAll(
        () -> assertEquals(0.0, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(0.0, moduleAccelerations.angle.getRadians(), kEpsilon));
  }

  @Test
  void testParameterizedConstructor() {
    var moduleAccelerations = new SwerveModuleAcceleration(2.5, new Rotation2d(1.5));

    assertAll(
        () -> assertEquals(2.5, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(1.5, moduleAccelerations.angle.getRadians(), kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var acceleration = MetersPerSecondPerSecond.of(3.0);
    var angle = new Rotation2d(2.0);
    var moduleAccelerations = new SwerveModuleAcceleration(acceleration, angle);

    assertAll(
        () -> assertEquals(3.0, moduleAccelerations.acceleration, kEpsilon),
        () -> assertEquals(2.0, moduleAccelerations.angle.getRadians(), kEpsilon));
  }

  @Test
  void testEquals() {
    var moduleAccelerations1 = new SwerveModuleAcceleration(2.0, new Rotation2d(1.5));
    var moduleAccelerations2 = new SwerveModuleAcceleration(2.0, new Rotation2d(1.5));
    var moduleAccelerations3 = new SwerveModuleAcceleration(2.1, new Rotation2d(1.5));

    assertEquals(moduleAccelerations1, moduleAccelerations2);
    assertNotEquals(moduleAccelerations1, moduleAccelerations3);
  }

  @Test
  void testCompareTo() {
    var slower = new SwerveModuleAcceleration(1.0, new Rotation2d(2.0));
    var faster = new SwerveModuleAcceleration(2.0, new Rotation2d(1.0));

    assertTrue(slower.compareTo(faster) < 0);
    assertTrue(faster.compareTo(slower) > 0);
    assertEquals(0, slower.compareTo(new SwerveModuleAcceleration(1.0, new Rotation2d(2.0))));
  }

  @Test
  void testInterpolate() {
    final var start = new SwerveModuleAcceleration(1.0, new Rotation2d(0.0));
    final var end = new SwerveModuleAcceleration(5.0, new Rotation2d(Math.PI / 2));

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.acceleration, kEpsilon),
        () -> assertEquals(0.0, atStart.angle.getRadians(), kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.acceleration, kEpsilon),
        () -> assertEquals(Math.PI / 2, atEnd.angle.getRadians(), kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.acceleration, kEpsilon),
        () -> assertEquals(Math.PI / 4, atMidpoint.angle.getRadians(), kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.acceleration, kEpsilon),
        () -> assertEquals(Math.PI / 8, atQuarter.angle.getRadians(), kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.acceleration, kEpsilon),
        () -> assertEquals(0.0, belowRange.angle.getRadians(), kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.acceleration, kEpsilon),
        () -> assertEquals(Math.PI / 2, aboveRange.angle.getRadians(), kEpsilon));
  }
}
