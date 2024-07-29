// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MecanumDriveWheelSpeedsTest {
  @Test
  void testPlus() {
    final var left = new MecanumDriveWheelSpeeds(1.0, 0.5, 2.0, 1.5);
    final var right = new MecanumDriveWheelSpeeds(2.0, 1.5, 0.5, 1.0);

    final var wheelSpeeds = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, wheelSpeeds.frontLeftMetersPerSecond),
        () -> assertEquals(2.0, wheelSpeeds.frontRightMetersPerSecond),
        () -> assertEquals(2.5, wheelSpeeds.rearLeftMetersPerSecond),
        () -> assertEquals(2.5, wheelSpeeds.rearRightMetersPerSecond));
  }

  @Test
  void testMinus() {
    final var left = new MecanumDriveWheelSpeeds(1.0, 0.5, 2.0, 1.5);
    final var right = new MecanumDriveWheelSpeeds(2.0, 0.5, 0.5, 1.0);

    final var wheelSpeeds = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, wheelSpeeds.frontLeftMetersPerSecond),
        () -> assertEquals(0.0, wheelSpeeds.frontRightMetersPerSecond),
        () -> assertEquals(1.5, wheelSpeeds.rearLeftMetersPerSecond),
        () -> assertEquals(0.5, wheelSpeeds.rearRightMetersPerSecond));
  }

  @Test
  void testUnaryMinus() {
    final var wheelSpeeds = new MecanumDriveWheelSpeeds(1.0, 0.5, 2.0, 1.5).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, wheelSpeeds.frontLeftMetersPerSecond),
        () -> assertEquals(-0.5, wheelSpeeds.frontRightMetersPerSecond),
        () -> assertEquals(-2.0, wheelSpeeds.rearLeftMetersPerSecond),
        () -> assertEquals(-1.5, wheelSpeeds.rearRightMetersPerSecond));
  }

  @Test
  void testMultiplication() {
    final var wheelSpeeds = new MecanumDriveWheelSpeeds(1.0, 0.5, 2.0, 1.5).times(2.0);

    assertAll(
        () -> assertEquals(2.0, wheelSpeeds.frontLeftMetersPerSecond),
        () -> assertEquals(1.0, wheelSpeeds.frontRightMetersPerSecond),
        () -> assertEquals(4.0, wheelSpeeds.rearLeftMetersPerSecond),
        () -> assertEquals(3.0, wheelSpeeds.rearRightMetersPerSecond));
  }

  @Test
  void testDivision() {
    final var wheelSpeeds = new MecanumDriveWheelSpeeds(1.0, 0.5, 2.0, 1.5).div(2.0);

    assertAll(
        () -> assertEquals(0.5, wheelSpeeds.frontLeftMetersPerSecond),
        () -> assertEquals(0.25, wheelSpeeds.frontRightMetersPerSecond),
        () -> assertEquals(1.0, wheelSpeeds.rearLeftMetersPerSecond),
        () -> assertEquals(0.75, wheelSpeeds.rearRightMetersPerSecond));
  }
}
