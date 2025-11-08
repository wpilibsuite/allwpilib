// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.units.Units;
import org.junit.jupiter.api.Test;

class Transform2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testNewWithMeasures() {
    var transform =
        new Transform2d(Units.Inches.of(6), Units.Inches.of(8), Rotation2d.fromDegrees(45));

    assertEquals(0.1524, transform.getX(), kEpsilon);
    assertEquals(0.2032, transform.getY(), kEpsilon);
    assertEquals(Math.PI / 4, transform.getRotation().getRadians(), kEpsilon);
  }

  @Test
  void testToMatrix() {
    var before = new Transform2d(1.0, 2.0, Rotation2d.fromDegrees(20.0));
    var after = new Transform2d(before.toMatrix());

    assertEquals(before, after);
  }

  @Test
  void testInverse() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transform = new Transform2d(new Translation2d(5.0, 0.0), Rotation2d.fromDegrees(5.0));

    var transformed = initial.plus(transform);
    var untransformed = transformed.plus(transform.inverse());

    assertEquals(initial, untransformed);
  }

  @Test
  void testComposition() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transform1 = new Transform2d(new Translation2d(5.0, 0.0), Rotation2d.fromDegrees(5.0));
    var transform2 = new Transform2d(new Translation2d(0.0, 2.0), Rotation2d.fromDegrees(5.0));

    var transformedSeparate = initial.plus(transform1).plus(transform2);
    var transformedCombined = initial.plus(transform1.plus(transform2));

    assertEquals(transformedSeparate, transformedCombined);
  }
}
