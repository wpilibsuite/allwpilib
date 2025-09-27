// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DifferentialDriveWheelAccelerationsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDefaultConstructor() {
    var wheelAccelerations = new DifferentialDriveWheelAccelerations();

    assertAll(
        () -> assertEquals(0.0, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(0.0, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testParameterizedConstructor() {
    var wheelAccelerations = new DifferentialDriveWheelAccelerations(1.5, 2.5);

    assertAll(
        () -> assertEquals(1.5, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(2.5, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var left = MetersPerSecondPerSecond.of(3.0);
    var right = MetersPerSecondPerSecond.of(4.5);
    var wheelAccelerations = new DifferentialDriveWheelAccelerations(left, right);

    assertAll(
        () -> assertEquals(3.0, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(4.5, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testPlus() {
    final var left = new DifferentialDriveWheelAccelerations(1.0, 0.5);
    final var right = new DifferentialDriveWheelAccelerations(2.0, 1.5);

    final var wheelAccelerations = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelAccelerations.left),
        () -> assertEquals(2.0, wheelAccelerations.right));
  }

  @Test
  void testMinus() {
    final var left = new DifferentialDriveWheelAccelerations(1.0, 0.5);
    final var right = new DifferentialDriveWheelAccelerations(2.0, 0.5);

    final var wheelAccelerations = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, wheelAccelerations.left),
        () -> assertEquals(0.0, wheelAccelerations.right));
  }

  @Test
  void testUnaryMinus() {
    final var wheelAccelerations = new DifferentialDriveWheelAccelerations(1.0, 0.5).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelAccelerations.left),
        () -> assertEquals(-0.5, wheelAccelerations.right));
  }

  @Test
  void testMultiplication() {
    final var wheelAccelerations = new DifferentialDriveWheelAccelerations(1.0, 0.5).times(2.0);

    assertAll(
        () -> assertEquals(2.0, wheelAccelerations.left),
        () -> assertEquals(1.0, wheelAccelerations.right));
  }

  @Test
  void testDivision() {
    final var wheelAccelerations = new DifferentialDriveWheelAccelerations(1.0, 0.5).div(2.0);

    assertAll(
        () -> assertEquals(0.5, wheelAccelerations.left),
        () -> assertEquals(0.25, wheelAccelerations.right));
  }

  @Test
  void testInterpolate() {
    final var start = new DifferentialDriveWheelAccelerations(1.0, 2.0);
    final var end = new DifferentialDriveWheelAccelerations(5.0, 6.0);

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.left, kEpsilon),
        () -> assertEquals(2.0, atStart.right, kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.left, kEpsilon),
        () -> assertEquals(6.0, atEnd.right, kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.left, kEpsilon),
        () -> assertEquals(4.0, atMidpoint.right, kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.left, kEpsilon),
        () -> assertEquals(3.0, atQuarter.right, kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.left, kEpsilon),
        () -> assertEquals(2.0, belowRange.right, kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.left, kEpsilon),
        () -> assertEquals(6.0, aboveRange.right, kEpsilon));
  }
}
