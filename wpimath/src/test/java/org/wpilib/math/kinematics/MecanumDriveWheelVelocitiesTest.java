// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MecanumDriveWheelVelocitiesTest {
  @Test
  void testPlus() {
    final var left = new MecanumDriveWheelVelocities(1.0, 0.5, 2.0, 1.5);
    final var right = new MecanumDriveWheelVelocities(2.0, 1.5, 0.5, 1.0);

    final var wheelVelocities = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelVelocities.frontLeft),
        () -> assertEquals(2.0, wheelVelocities.frontRight),
        () -> assertEquals(2.5, wheelVelocities.rearLeft),
        () -> assertEquals(2.5, wheelVelocities.rearRight));
  }

  @Test
  void testMinus() {
    final var left = new MecanumDriveWheelVelocities(1.0, 0.5, 2.0, 1.5);
    final var right = new MecanumDriveWheelVelocities(2.0, 0.5, 0.5, 1.0);

    final var wheelVelocities = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, wheelVelocities.frontLeft),
        () -> assertEquals(0.0, wheelVelocities.frontRight),
        () -> assertEquals(1.5, wheelVelocities.rearLeft),
        () -> assertEquals(0.5, wheelVelocities.rearRight));
  }

  @Test
  void testUnaryMinus() {
    final var wheelVelocities = new MecanumDriveWheelVelocities(1.0, 0.5, 2.0, 1.5).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelVelocities.frontLeft),
        () -> assertEquals(-0.5, wheelVelocities.frontRight),
        () -> assertEquals(-2.0, wheelVelocities.rearLeft),
        () -> assertEquals(-1.5, wheelVelocities.rearRight));
  }

  @Test
  void testMultiplication() {
    final var wheelVelocities = new MecanumDriveWheelVelocities(1.0, 0.5, 2.0, 1.5).times(2.0);

    assertAll(
        () -> assertEquals(2.0, wheelVelocities.frontLeft),
        () -> assertEquals(1.0, wheelVelocities.frontRight),
        () -> assertEquals(4.0, wheelVelocities.rearLeft),
        () -> assertEquals(3.0, wheelVelocities.rearRight));
  }

  @Test
  void testDivision() {
    final var wheelVelocities = new MecanumDriveWheelVelocities(1.0, 0.5, 2.0, 1.5).div(2.0);

    assertAll(
        () -> assertEquals(0.5, wheelVelocities.frontLeft),
        () -> assertEquals(0.25, wheelVelocities.frontRight),
        () -> assertEquals(1.0, wheelVelocities.rearLeft),
        () -> assertEquals(0.75, wheelVelocities.rearRight));
  }

  @Test
  void testInterpolate() {
    final var start = new MecanumDriveWheelVelocities(1.0, 2.0, 3.0, 4.0);
    final var end = new MecanumDriveWheelVelocities(5.0, 6.0, 7.0, 8.0);

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.frontLeft, 1e-9),
        () -> assertEquals(2.0, atStart.frontRight, 1e-9),
        () -> assertEquals(3.0, atStart.rearLeft, 1e-9),
        () -> assertEquals(4.0, atStart.rearRight, 1e-9));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.frontLeft, 1e-9),
        () -> assertEquals(6.0, atEnd.frontRight, 1e-9),
        () -> assertEquals(7.0, atEnd.rearLeft, 1e-9),
        () -> assertEquals(8.0, atEnd.rearRight, 1e-9));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.frontLeft, 1e-9),
        () -> assertEquals(4.0, atMidpoint.frontRight, 1e-9),
        () -> assertEquals(5.0, atMidpoint.rearLeft, 1e-9),
        () -> assertEquals(6.0, atMidpoint.rearRight, 1e-9));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.frontLeft, 1e-9),
        () -> assertEquals(3.0, atQuarter.frontRight, 1e-9),
        () -> assertEquals(4.0, atQuarter.rearLeft, 1e-9),
        () -> assertEquals(5.0, atQuarter.rearRight, 1e-9));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.frontLeft, 1e-9),
        () -> assertEquals(2.0, belowRange.frontRight, 1e-9),
        () -> assertEquals(3.0, belowRange.rearLeft, 1e-9),
        () -> assertEquals(4.0, belowRange.rearRight, 1e-9));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.frontLeft, 1e-9),
        () -> assertEquals(6.0, aboveRange.frontRight, 1e-9),
        () -> assertEquals(7.0, aboveRange.rearLeft, 1e-9),
        () -> assertEquals(8.0, aboveRange.rearRight, 1e-9));
  }
}
