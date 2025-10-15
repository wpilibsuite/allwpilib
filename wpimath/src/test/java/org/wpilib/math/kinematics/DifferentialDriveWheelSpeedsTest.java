// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DifferentialDriveWheelSpeedsTest {
  @Test
  void testPlus() {
    final var left = new DifferentialDriveWheelSpeeds(1.0, 0.5);
    final var right = new DifferentialDriveWheelSpeeds(2.0, 1.5);

    final var wheelSpeeds = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelSpeeds.left), () -> assertEquals(2.0, wheelSpeeds.right));
  }

  @Test
  void testMinus() {
    final var left = new DifferentialDriveWheelSpeeds(1.0, 0.5);
    final var right = new DifferentialDriveWheelSpeeds(2.0, 0.5);

    final var wheelSpeeds = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, wheelSpeeds.left), () -> assertEquals(0.0, wheelSpeeds.right));
  }

  @Test
  void testUnaryMinus() {
    final var wheelSpeeds = new DifferentialDriveWheelSpeeds(1.0, 0.5).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelSpeeds.left), () -> assertEquals(-0.5, wheelSpeeds.right));
  }

  @Test
  void testMultiplication() {
    final var wheelSpeeds = new DifferentialDriveWheelSpeeds(1.0, 0.5).times(2.0);

    assertAll(
        () -> assertEquals(2.0, wheelSpeeds.left), () -> assertEquals(1.0, wheelSpeeds.right));
  }

  @Test
  void testDivision() {
    final var wheelSpeeds = new DifferentialDriveWheelSpeeds(1.0, 0.5).div(2.0);

    assertAll(
        () -> assertEquals(0.5, wheelSpeeds.left), () -> assertEquals(0.25, wheelSpeeds.right));
  }
}
