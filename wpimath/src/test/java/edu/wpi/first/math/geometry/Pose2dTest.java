// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import org.junit.jupiter.api.Test;

class Pose2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testTransformBy() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transformation = new Transform2d(new Translation2d(5.0, 0.0), Rotation2d.fromDegrees(5.0));

    var transformed = initial.plus(transformation);

    assertAll(
        () -> assertEquals(1.0 + 5.0 / Math.sqrt(2.0), transformed.getX(), kEpsilon),
        () -> assertEquals(2.0 + 5.0 / Math.sqrt(2.0), transformed.getY(), kEpsilon),
        () -> assertEquals(50.0, transformed.getRotation().getDegrees(), kEpsilon));
  }

  @Test
  void testRelativeTo() {
    var initial = new Pose2d(0.0, 0.0, Rotation2d.fromDegrees(45.0));
    var last = new Pose2d(5.0, 5.0, Rotation2d.fromDegrees(45.0));

    var finalRelativeToInitial = last.relativeTo(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), finalRelativeToInitial.getX(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getY(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getRotation().getDegrees(), kEpsilon));
  }

  @Test
  void testEquality() {
    var one = new Pose2d(0.0, 5.0, Rotation2d.fromDegrees(43.0));
    var two = new Pose2d(0.0, 5.0, Rotation2d.fromDegrees(43.0));
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = new Pose2d(0.0, 5.0, Rotation2d.fromDegrees(43.0));
    var two = new Pose2d(0.0, 1.524, Rotation2d.fromDegrees(43.0));
    assertNotEquals(one, two);
  }

  @Test
  void testMinus() {
    var initial = new Pose2d(0.0, 0.0, Rotation2d.fromDegrees(45.0));
    var last = new Pose2d(5.0, 5.0, Rotation2d.fromDegrees(45.0));

    final var transform = last.minus(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), transform.getX(), kEpsilon),
        () -> assertEquals(0.0, transform.getY(), kEpsilon),
        () -> assertEquals(0.0, transform.getRotation().getDegrees(), kEpsilon));
  }
}
