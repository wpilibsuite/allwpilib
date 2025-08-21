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
}
