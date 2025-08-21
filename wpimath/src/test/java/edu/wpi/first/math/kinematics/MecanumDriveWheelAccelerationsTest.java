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
    var wheelAccelerations = new MecanumDriveWheelAccelerations(frontLeft, frontRight, rearLeft, rearRight);

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
    final var wheelAccelerations = new MecanumDriveWheelAccelerations(1.0, -2.0, 3.0, -4.0).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelAccelerations.frontLeft),
        () -> assertEquals(2.0, wheelAccelerations.frontRight),
        () -> assertEquals(-3.0, wheelAccelerations.rearLeft),
        () -> assertEquals(4.0, wheelAccelerations.rearRight));
  }

  @Test
  void testMultiplication() {
    final var wheelAccelerations = new MecanumDriveWheelAccelerations(2.0, 2.5, 3.0, 3.5).times(2.0);

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
}
