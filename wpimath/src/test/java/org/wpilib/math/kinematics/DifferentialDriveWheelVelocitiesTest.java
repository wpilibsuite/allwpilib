// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DifferentialDriveWheelVelocitiesTest {
  @Test
  void testPlus() {
    final var left = new DifferentialDriveWheelVelocities(1.0, 0.5);
    final var right = new DifferentialDriveWheelVelocities(2.0, 1.5);

    final var wheelVelocities = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelVelocities.left),
        () -> assertEquals(2.0, wheelVelocities.right));
  }

  @Test
  void testMinus() {
    final var left = new DifferentialDriveWheelVelocities(1.0, 0.5);
    final var right = new DifferentialDriveWheelVelocities(2.0, 0.5);

    final var wheelVelocities = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, wheelVelocities.left),
        () -> assertEquals(0.0, wheelVelocities.right));
  }

  @Test
  void testUnaryMinus() {
    final var wheelVelocities = new DifferentialDriveWheelVelocities(1.0, 0.5).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelVelocities.left),
        () -> assertEquals(-0.5, wheelVelocities.right));
  }

  @Test
  void testMultiplication() {
    final var wheelVelocities = new DifferentialDriveWheelVelocities(1.0, 0.5).times(2.0);

    assertAll(
        () -> assertEquals(2.0, wheelVelocities.left),
        () -> assertEquals(1.0, wheelVelocities.right));
  }

  @Test
  void testDivision() {
    final var wheelVelocities = new DifferentialDriveWheelVelocities(1.0, 0.5).div(2.0);

    assertAll(
        () -> assertEquals(0.5, wheelVelocities.left),
        () -> assertEquals(0.25, wheelVelocities.right));
  }

  @Test
  void testInterpolate() {
    final var start = new DifferentialDriveWheelVelocities(1.0, 2.0);
    final var end = new DifferentialDriveWheelVelocities(5.0, 6.0);

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.left, 1e-9), () -> assertEquals(2.0, atStart.right, 1e-9));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.left, 1e-9), () -> assertEquals(6.0, atEnd.right, 1e-9));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.left, 1e-9),
        () -> assertEquals(4.0, atMidpoint.right, 1e-9));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.left, 1e-9),
        () -> assertEquals(3.0, atQuarter.right, 1e-9));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.left, 1e-9),
        () -> assertEquals(2.0, belowRange.right, 1e-9));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.left, 1e-9),
        () -> assertEquals(6.0, aboveRange.right, 1e-9));
  }
}
