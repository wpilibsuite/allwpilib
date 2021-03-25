// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class Transform2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testInverse() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transform = new Transform2d(new Translation2d(5.0, 0.0), Rotation2d.fromDegrees(5.0));

    var transformed = initial.plus(transform);
    var untransformed = transformed.plus(transform.inverse());

    assertAll(
        () -> assertEquals(initial.getX(), untransformed.getX(), kEpsilon),
        () -> assertEquals(initial.getY(), untransformed.getY(), kEpsilon),
        () ->
            assertEquals(
                initial.getRotation().getDegrees(),
                untransformed.getRotation().getDegrees(),
                kEpsilon));
  }

  @Test
  void testComposition() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transform1 = new Transform2d(new Translation2d(5.0, 0.0), Rotation2d.fromDegrees(5.0));
    var transform2 = new Transform2d(new Translation2d(0.0, 2.0), Rotation2d.fromDegrees(5.0));

    var transformedSeparate = initial.plus(transform1).plus(transform2);
    var transformedCombined = initial.plus(transform1.plus(transform2));

    assertAll(
        () -> assertEquals(transformedSeparate.getX(), transformedCombined.getX(), kEpsilon),
        () -> assertEquals(transformedSeparate.getY(), transformedCombined.getY(), kEpsilon),
        () ->
            assertEquals(
                transformedSeparate.getRotation().getDegrees(),
                transformedCombined.getRotation().getDegrees(),
                kEpsilon));
  }
}
