/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

class Pose2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testTransformBy() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transformation = new Transform2d(new Translation2d(5.0, 0.0),
        Rotation2d.fromDegrees(5.0));

    var transformed = initial.plus(transformation);

    assertAll(
        () -> assertEquals(transformed.getX(), 1 + 5.0 / Math.sqrt(2.0), kEpsilon),
        () -> assertEquals(transformed.getY(), 2 + 5.0 / Math.sqrt(2.0), kEpsilon),
        () -> assertEquals(transformed.getRotation().getDegrees(), 50.0, kEpsilon)
    );
  }

  @Test
  void testRelativeTo() {
    var initial = new Pose2d(0.0, 0.0, Rotation2d.fromDegrees(45.0));
    var last = new Pose2d(5.0, 5.0, Rotation2d.fromDegrees(45.0));

    var finalRelativeToInitial = last.relativeTo(initial);

    assertAll(
        () -> assertEquals(finalRelativeToInitial.getX(), 5.0 * Math.sqrt(2.0),
            kEpsilon),
        () -> assertEquals(finalRelativeToInitial.getY(), 0.0, kEpsilon),
        () -> assertEquals(finalRelativeToInitial.getRotation().getDegrees(), 0.0, kEpsilon)
    );
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

  void testMinus() {
    var initial = new Pose2d(0.0, 0.0, Rotation2d.fromDegrees(45.0));
    var last = new Pose2d(5.0, 5.0, Rotation2d.fromDegrees(45.0));

    final var transform = last.minus(initial);

    assertAll(
        () -> assertEquals(transform.getX(), 5.0 * Math.sqrt(2.0), kEpsilon),
        () -> assertEquals(transform.getY(), 0.0, kEpsilon),
        () -> assertEquals(transform.getRotation().getDegrees(), 0.0, kEpsilon)
    );
  }
}
