// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class Transform3dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testInverse() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial =
        new Pose3d(
            new Translation3d(1.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var transform =
        new Transform3d(
            new Translation3d(5.0, 0.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));

    var transformed = initial.plus(transform);
    var untransformed = transformed.plus(transform.inverse());

    assertAll(
        () -> assertEquals(initial.getX(), untransformed.getX(), kEpsilon),
        () -> assertEquals(initial.getY(), untransformed.getY(), kEpsilon),
        () -> assertEquals(initial.getZ(), untransformed.getZ(), kEpsilon),
        () ->
            assertEquals(
                initial.getRotation().getZ(), untransformed.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testComposition() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial =
        new Pose3d(
            new Translation3d(1.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var transform1 =
        new Transform3d(
            new Translation3d(5.0, 0.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));
    var transform2 =
        new Transform3d(
            new Translation3d(0.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));

    var transformedSeparate = initial.plus(transform1).plus(transform2);
    var transformedCombined = initial.plus(transform1.plus(transform2));

    assertAll(
        () -> assertEquals(transformedSeparate.getX(), transformedCombined.getX(), kEpsilon),
        () -> assertEquals(transformedSeparate.getY(), transformedCombined.getY(), kEpsilon),
        () -> assertEquals(transformedSeparate.getZ(), transformedCombined.getZ(), kEpsilon),
        () ->
            assertEquals(
                transformedSeparate.getRotation().getZ(),
                transformedCombined.getRotation().getZ(),
                kEpsilon));
  }
}
