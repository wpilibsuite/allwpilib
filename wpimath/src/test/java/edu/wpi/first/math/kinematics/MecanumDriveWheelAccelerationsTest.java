// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static edu.wpi.first.units.Units.MetersPerSecondPerSecond;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MecanumDriveWheelAccelerationsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDefaultConstructor() {
    var wheelAccelerations = new MecanumDriveWheelAccelerations();

    assertAll(
        () -> assertEquals(0.0, wheelAccelerations.frontLeft, kEpsilon),
        () -> assertEquals(0.0, wheelAccelerations.frontRight, kEpsilon),
        () -> assertEquals(0.0, wheelAccelerations.rearLeft, kEpsilon),
        () -> assertEquals(0.0, wheelAccelerations.rearRight, kEpsilon));
  }

  @Test
  void testParameterizedConstructor() {
    var wheelAccelerations = new MecanumDriveWheelAccelerations(1.0, 2.0, 3.0, 4.0);

    assertAll(
        () -> assertEquals(1.0, wheelAccelerations.frontLeft, kEpsilon),
        () -> assertEquals(2.0, wheelAccelerations.frontRight, kEpsilon),
        () -> assertEquals(3.0, wheelAccelerations.rearLeft, kEpsilon),
        () -> assertEquals(4.0, wheelAccelerations.rearRight, kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var frontLeft = MetersPerSecondPerSecond.of(1.5);
    var frontRight = MetersPerSecondPerSecond.of(2.5);
    var rearLeft = MetersPerSecondPerSecond.of(3.5);
    var rearRight = MetersPerSecondPerSecond.of(4.5);
    var wheelAccelerations =
        new MecanumDriveWheelAccelerations(frontLeft, frontRight, rearLeft, rearRight);

    assertAll(
        () -> assertEquals(1.5, wheelAccelerations.frontLeft, kEpsilon),
        () -> assertEquals(2.5, wheelAccelerations.frontRight, kEpsilon),
        () -> assertEquals(3.5, wheelAccelerations.rearLeft, kEpsilon),
        () -> assertEquals(4.5, wheelAccelerations.rearRight, kEpsilon));
  }

  @Test
  void testPlus() {
    final var left = new MecanumDriveWheelAccelerations(1.0, 0.5, 2.0, 1.5);
    final var right = new MecanumDriveWheelAccelerations(2.0, 1.5, 0.5, 1.0);

    final var wheelAccelerations = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelAccelerations.frontLeft),
        () -> assertEquals(2.0, wheelAccelerations.frontRight),
        () -> assertEquals(2.5, wheelAccelerations.rearLeft),
        () -> assertEquals(2.5, wheelAccelerations.rearRight));
  }

  @Test
  void testMinus() {
    final var left = new MecanumDriveWheelAccelerations(5.0, 4.0, 6.0, 2.5);
    final var right = new MecanumDriveWheelAccelerations(1.0, 2.0, 3.0, 0.5);

    final var wheelAccelerations = left.minus(right);

    assertAll(
        () -> assertEquals(4.0, wheelAccelerations.frontLeft),
        () -> assertEquals(2.0, wheelAccelerations.frontRight),
        () -> assertEquals(3.0, wheelAccelerations.rearLeft),
        () -> assertEquals(2.0, wheelAccelerations.rearRight));
  }

  @Test
  void testUnaryMinus() {
    final var wheelAccelerations =
        new MecanumDriveWheelAccelerations(1.0, -2.0, 3.0, -4.0).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelAccelerations.frontLeft),
        () -> assertEquals(2.0, wheelAccelerations.frontRight),
        () -> assertEquals(-3.0, wheelAccelerations.rearLeft),
        () -> assertEquals(4.0, wheelAccelerations.rearRight));
  }

  @Test
  void testMultiplication() {
    final var wheelAccelerations =
        new MecanumDriveWheelAccelerations(2.0, 2.5, 3.0, 3.5).times(2.0);

    assertAll(
        () -> assertEquals(4.0, wheelAccelerations.frontLeft),
        () -> assertEquals(5.0, wheelAccelerations.frontRight),
        () -> assertEquals(6.0, wheelAccelerations.rearLeft),
        () -> assertEquals(7.0, wheelAccelerations.rearRight));
  }

  @Test
  void testDivision() {
    final var wheelAccelerations = new MecanumDriveWheelAccelerations(2.0, 2.5, 1.5, 1.0).div(2.0);

    assertAll(
        () -> assertEquals(1.0, wheelAccelerations.frontLeft),
        () -> assertEquals(1.25, wheelAccelerations.frontRight),
        () -> assertEquals(0.75, wheelAccelerations.rearLeft),
        () -> assertEquals(0.5, wheelAccelerations.rearRight));
  }

  @Test
  void testInterpolate() {
    final var start = new MecanumDriveWheelAccelerations(1.0, 2.0, 3.0, 4.0);
    final var end = new MecanumDriveWheelAccelerations(5.0, 6.0, 7.0, 8.0);

    // Test interpolation at t=0 (should return start)
    final var atStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, atStart.frontLeft, kEpsilon),
        () -> assertEquals(2.0, atStart.frontRight, kEpsilon),
        () -> assertEquals(3.0, atStart.rearLeft, kEpsilon),
        () -> assertEquals(4.0, atStart.rearRight, kEpsilon));

    // Test interpolation at t=1 (should return end)
    final var atEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(5.0, atEnd.frontLeft, kEpsilon),
        () -> assertEquals(6.0, atEnd.frontRight, kEpsilon),
        () -> assertEquals(7.0, atEnd.rearLeft, kEpsilon),
        () -> assertEquals(8.0, atEnd.rearRight, kEpsilon));

    // Test interpolation at t=0.5 (should return midpoint)
    final var atMidpoint = start.interpolate(end, 0.5);
    assertAll(
        () -> assertEquals(3.0, atMidpoint.frontLeft, kEpsilon),
        () -> assertEquals(4.0, atMidpoint.frontRight, kEpsilon),
        () -> assertEquals(5.0, atMidpoint.rearLeft, kEpsilon),
        () -> assertEquals(6.0, atMidpoint.rearRight, kEpsilon));

    // Test interpolation at t=0.25
    final var atQuarter = start.interpolate(end, 0.25);
    assertAll(
        () -> assertEquals(2.0, atQuarter.frontLeft, kEpsilon),
        () -> assertEquals(3.0, atQuarter.frontRight, kEpsilon),
        () -> assertEquals(4.0, atQuarter.rearLeft, kEpsilon),
        () -> assertEquals(5.0, atQuarter.rearRight, kEpsilon));

    // Test clamping: t < 0 should clamp to 0
    final var belowRange = start.interpolate(end, -0.5);
    assertAll(
        () -> assertEquals(1.0, belowRange.frontLeft, kEpsilon),
        () -> assertEquals(2.0, belowRange.frontRight, kEpsilon),
        () -> assertEquals(3.0, belowRange.rearLeft, kEpsilon),
        () -> assertEquals(4.0, belowRange.rearRight, kEpsilon));

    // Test clamping: t > 1 should clamp to 1
    final var aboveRange = start.interpolate(end, 1.5);
    assertAll(
        () -> assertEquals(5.0, aboveRange.frontLeft, kEpsilon),
        () -> assertEquals(6.0, aboveRange.frontRight, kEpsilon),
        () -> assertEquals(7.0, aboveRange.rearLeft, kEpsilon),
        () -> assertEquals(8.0, aboveRange.rearRight, kEpsilon));
  }
}
