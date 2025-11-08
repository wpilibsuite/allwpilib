// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.wpilib.units.Units.InchesPerSecond;
import static org.wpilib.units.Units.RPM;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Twist2d;
import org.junit.jupiter.api.Test;

class ChassisSpeedsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testDiscretize() {
    final var target = new ChassisSpeeds(1.0, 0.0, 0.5);
    final var duration = 1.0;
    final var dt = 0.01;

    final var speeds = target.discretize(duration);
    final var twist = new Twist2d(speeds.vx * dt, speeds.vy * dt, speeds.omega * dt);

    var pose = Pose2d.kZero;
    for (double time = 0; time < duration; time += dt) {
      pose = pose.plus(twist.exp());
    }

    final var result = pose; // For lambda capture
    assertAll(
        () -> assertEquals(target.vx * duration, result.getX(), kEpsilon),
        () -> assertEquals(target.vy * duration, result.getY(), kEpsilon),
        () -> assertEquals(target.omega * duration, result.getRotation().getRadians(), kEpsilon));
  }

  @Test
  void testMeasureConstructor() {
    var vx = InchesPerSecond.of(14.52);
    var vy = InchesPerSecond.of(0);
    var omega = RPM.of(0.02);
    var speeds = new ChassisSpeeds(vx, vy, omega);

    assertAll(
        () -> assertEquals(0.368808, speeds.vx, kEpsilon),
        () -> assertEquals(0, speeds.vy, kEpsilon),
        () -> assertEquals(0.002094395102, speeds.omega, kEpsilon));
  }

  @Test
  void testToRobotRelative() {
    final var chassisSpeeds = new ChassisSpeeds(1.0, 0.0, 0.5).toRobotRelative(Rotation2d.kCW_Pi_2);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(1.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.5, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testToFieldRelative() {
    final var chassisSpeeds =
        new ChassisSpeeds(1.0, 0.0, 0.5).toFieldRelative(Rotation2d.fromDegrees(45.0));

    assertAll(
        () -> assertEquals(1.0 / Math.sqrt(2.0), chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(1.0 / Math.sqrt(2.0), chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.5, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testPlus() {
    final var left = new ChassisSpeeds(1.0, 0.5, 0.75);
    final var right = new ChassisSpeeds(2.0, 1.5, 0.25);

    final var chassisSpeeds = left.plus(right);

    assertAll(
        () -> assertEquals(3.0, chassisSpeeds.vx),
        () -> assertEquals(2.0, chassisSpeeds.vy),
        () -> assertEquals(1.0, chassisSpeeds.omega));
  }

  @Test
  void testMinus() {
    final var left = new ChassisSpeeds(1.0, 0.5, 0.75);
    final var right = new ChassisSpeeds(2.0, 0.5, 0.25);

    final var chassisSpeeds = left.minus(right);

    assertAll(
        () -> assertEquals(-1.0, chassisSpeeds.vx),
        () -> assertEquals(0.0, chassisSpeeds.vy),
        () -> assertEquals(0.5, chassisSpeeds.omega));
  }

  @Test
  void testUnaryMinus() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).unaryMinus();

    assertAll(
        () -> assertEquals(-1.0, chassisSpeeds.vx),
        () -> assertEquals(-0.5, chassisSpeeds.vy),
        () -> assertEquals(-0.75, chassisSpeeds.omega));
  }

  @Test
  void testMultiplication() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).times(2.0);

    assertAll(
        () -> assertEquals(2.0, chassisSpeeds.vx),
        () -> assertEquals(1.0, chassisSpeeds.vy),
        () -> assertEquals(1.5, chassisSpeeds.omega));
  }

  @Test
  void testDivision() {
    final var chassisSpeeds = (new ChassisSpeeds(1.0, 0.5, 0.75)).div(2.0);

    assertAll(
        () -> assertEquals(0.5, chassisSpeeds.vx),
        () -> assertEquals(0.25, chassisSpeeds.vy),
        () -> assertEquals(0.375, chassisSpeeds.omega));
  }
}
