// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.units.Units.MetersPerSecondPerSecond;
import static org.wpilib.units.Units.RadiansPerSecondPerSecond;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;

class ChassisAccelerationsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDefaultConstructor() {
    var accelerations = new ChassisAccelerations();

    assertAll(
        () -> assertEquals(0.0, accelerations.ax, kEpsilon),
        () -> assertEquals(0.0, accelerations.ay, kEpsilon),
        () -> assertEquals(0.0, accelerations.alpha, kEpsilon));
  }

  @Test
  void testParameterizedConstructor() {
    var accelerations = new ChassisAccelerations(1.0, 2.0, 3.0);

    assertAll(
        () -> assertEquals(1.0, accelerations.ax, kEpsilon),
        () -> assertEquals(2.0, accelerations.ay, kEpsilon),
        () -> assertEquals(3.0, accelerations.alpha, kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var ax = MetersPerSecondPerSecond.of(2.5);
    var ay = MetersPerSecondPerSecond.of(1.5);
    var alpha = RadiansPerSecondPerSecond.of(0.75);
    var accelerations = new ChassisAccelerations(ax, ay, alpha);

    assertAll(
        () -> assertEquals(2.5, accelerations.ax, kEpsilon),
        () -> assertEquals(1.5, accelerations.ay, kEpsilon),
        () -> assertEquals(0.75, accelerations.alpha, kEpsilon));
  }

  @Test
  void testToRobotRelative() {
    final var chassisAccelerations =
        new ChassisAccelerations(1.0, 0.0, 0.5).toRobotRelative(Rotation2d.kCW_Pi_2);

    assertAll(
        () -> assertEquals(0.0, chassisAccelerations.ax, kEpsilon),
        () -> assertEquals(1.0, chassisAccelerations.ay, kEpsilon),
        () -> assertEquals(0.5, chassisAccelerations.alpha, kEpsilon));
  }

  @Test
  void testToFieldRelative() {
    final var chassisAccelerations =
        new ChassisAccelerations(1.0, 0.0, 0.5).toFieldRelative(Rotation2d.fromDegrees(45.0));

    assertAll(
        () -> assertEquals(1.0 / Math.sqrt(2.0), chassisAccelerations.ax, kEpsilon),
        () -> assertEquals(1.0 / Math.sqrt(2.0), chassisAccelerations.ay, kEpsilon),
        () -> assertEquals(0.5, chassisAccelerations.alpha, kEpsilon));
  }

  @Test
  void testPlus() {
    final var left = new ChassisAccelerations(1.0, 0.5, 0.75);
    final var right = new ChassisAccelerations(2.0, 1.5, 0.25);

    final var chassisAccelerations = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, chassisAccelerations.ax),
        () -> assertEquals(2.0, chassisAccelerations.ay),
        () -> assertEquals(1.0, chassisAccelerations.alpha));
  }

  @Test
  void testMinus() {
    final var left = new ChassisAccelerations(1.0, 0.5, 0.75);
    final var right = new ChassisAccelerations(2.0, 0.5, 0.25);

    final var chassisAccelerations = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, chassisAccelerations.ax),
        () -> assertEquals(0.0, chassisAccelerations.ay),
        () -> assertEquals(0.5, chassisAccelerations.alpha));
  }

  @Test
  void testUnaryMinus() {
    final var chassisAccelerations = new ChassisAccelerations(1.0, 0.5, 0.75).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, chassisAccelerations.ax),
        () -> assertEquals(-0.5, chassisAccelerations.ay),
        () -> assertEquals(-0.75, chassisAccelerations.alpha));
  }

  @Test
  void testMultiplication() {
    final var chassisAccelerations = new ChassisAccelerations(1.0, 0.5, 0.75).times(2.0);

    assertAll(
        () -> assertEquals(2.0, chassisAccelerations.ax),
        () -> assertEquals(1.0, chassisAccelerations.ay),
        () -> assertEquals(1.5, chassisAccelerations.alpha));
  }

  @Test
  void testDivision() {
    final var chassisAccelerations = new ChassisAccelerations(2.0, 1.0, 1.5).div(2.0);

    assertAll(
        () -> assertEquals(1.0, chassisAccelerations.ax),
        () -> assertEquals(0.5, chassisAccelerations.ay),
        () -> assertEquals(0.75, chassisAccelerations.alpha));
  }

  @Test
  void testInterpolation() {
    var start = new ChassisAccelerations(0.0, 0.0, 0.0);
    var end = new ChassisAccelerations(10.0, 20.0, 30.0);
    var result = start.interpolate(end, 0.5);

    assertAll(
        () -> assertEquals(5.0, result.ax, kEpsilon),
        () -> assertEquals(10.0, result.ay, kEpsilon),
        () -> assertEquals(15.0, result.alpha, kEpsilon));
  }

  @Test
  void testInterpolationAtBounds() {
    var start = new ChassisAccelerations(1.0, 2.0, 3.0);
    var end = new ChassisAccelerations(4.0, 5.0, 6.0);

    // Test t = 0 (should return start)
    var resultStart = start.interpolate(end, 0.0);
    assertAll(
        () -> assertEquals(1.0, resultStart.ax, kEpsilon),
        () -> assertEquals(2.0, resultStart.ay, kEpsilon),
        () -> assertEquals(3.0, resultStart.alpha, kEpsilon));

    // Test t = 1 (should return end)
    var resultEnd = start.interpolate(end, 1.0);
    assertAll(
        () -> assertEquals(4.0, resultEnd.ax, kEpsilon),
        () -> assertEquals(5.0, resultEnd.ay, kEpsilon),
        () -> assertEquals(6.0, resultEnd.alpha, kEpsilon));
  }
}
