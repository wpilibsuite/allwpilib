// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class ChassisSpeedsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testFieldRelativeConstruction() {
    final var chassisSpeeds =
        ChassisSpeeds.fromFieldRelativeSpeeds(1.0, 0.0, 0.5, Rotation2d.fromDegrees(-90.0));

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(1.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.5, chassisSpeeds.omegaRadiansPerSecond, kEpsilon));
  }

  @Test
  void testPlus() {
    final var left = new ChassisSpeeds(1.0, 0.5, 0.75);
    final var right = new ChassisSpeeds(2.0, 1.5, 0.25);

    final var chassisSpeeds = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, chassisSpeeds.vxMetersPerSecond),
        () -> assertEquals(2.0, chassisSpeeds.vyMetersPerSecond),
        () -> assertEquals(1.0, chassisSpeeds.omegaRadiansPerSecond));
  }

  @Test
  void testMinus() {
    final var left = new ChassisSpeeds(1.0, 0.5, 0.75);
    final var right = new ChassisSpeeds(2.0, 0.5, 0.25);

    final var chassisSpeeds = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, chassisSpeeds.vxMetersPerSecond),
        () -> assertEquals(0.0, chassisSpeeds.vyMetersPerSecond),
        () -> assertEquals(0.5, chassisSpeeds.omegaRadiansPerSecond));
  }

  @Test
  void testUnaryMinus() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, chassisSpeeds.vxMetersPerSecond),
        () -> assertEquals(-0.5, chassisSpeeds.vyMetersPerSecond),
        () -> assertEquals(-0.75, chassisSpeeds.omegaRadiansPerSecond));
  }

  @Test
  void testTimes() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).times(2.0);

    assertAll(
        () -> assertEquals(2.0, chassisSpeeds.vxMetersPerSecond),
        () -> assertEquals(1.0, chassisSpeeds.vyMetersPerSecond),
        () -> assertEquals(1.5, chassisSpeeds.omegaRadiansPerSecond));
  }

  @Test
  void testDiv() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).div(2.0);

    assertAll(
        () -> assertEquals(0.5, chassisSpeeds.vxMetersPerSecond),
        () -> assertEquals(0.25, chassisSpeeds.vyMetersPerSecond),
        () -> assertEquals(0.375, chassisSpeeds.omegaRadiansPerSecond));
  }
}
