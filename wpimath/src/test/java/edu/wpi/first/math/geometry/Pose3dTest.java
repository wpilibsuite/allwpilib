// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class Pose3dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testTransformByRotations() {
    var initialPose =
        new Pose3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0)));

    var transform1 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(90.0),
                Units.degreesToRadians(45.0),
                Units.degreesToRadians(0.0)));

    var transform2 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(-90.0),
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(0.0)));

    var transform3 =
        new Transform3d(
            new Translation3d(0.0, 0.0, 0.0),
            new Rotation3d(
                Units.degreesToRadians(0.0),
                Units.degreesToRadians(-45.0),
                Units.degreesToRadians(0.0)));

    // This sequence of rotations should diverge from the origin and eventually return to it. When
    // each rotation occurs, it should be performed intrinsicly, i.e. 'from the viewpoint' of and
    // with
    // the axes of the pose that is being transformed, just like how the translation is done 'from
    // the
    // viewpoint' of the pose that is being transformed.
    var finalPose =
        initialPose.transformBy(transform1).transformBy(transform2).transformBy(transform3);

    assertAll(
        () ->
            assertEquals(
                finalPose.getRotation().getX(), initialPose.getRotation().getX(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getY(), initialPose.getRotation().getY(), kEpsilon),
        () ->
            assertEquals(
                finalPose.getRotation().getZ(), initialPose.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testTransformBy() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial =
        new Pose3d(
            new Translation3d(1.0, 2.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var transformation =
        new Transform3d(
            new Translation3d(5.0, 0.0, 0.0), new Rotation3d(zAxis, Units.degreesToRadians(5.0)));

    var transformed = initial.plus(transformation);

    assertAll(
        () -> assertEquals(1.0 + 5.0 / Math.sqrt(2.0), transformed.getX(), kEpsilon),
        () -> assertEquals(2.0 + 5.0 / Math.sqrt(2.0), transformed.getY(), kEpsilon),
        () ->
            assertEquals(Units.degreesToRadians(50.0), transformed.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testRelativeTo() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    var finalRelativeToInitial = last.relativeTo(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), finalRelativeToInitial.getX(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getY(), kEpsilon),
        () -> assertEquals(0.0, finalRelativeToInitial.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testEquality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Pose3d(0.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    var two = new Pose3d(0.0, 1.524, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(43.0)));
    assertNotEquals(one, two);
  }

  @Test
  void testMinus() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var initial = new Pose3d(0.0, 0.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var last = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(45.0)));

    final var transform = last.minus(initial);

    assertAll(
        () -> assertEquals(5.0 * Math.sqrt(2.0), transform.getX(), kEpsilon),
        () -> assertEquals(0.0, transform.getY(), kEpsilon),
        () -> assertEquals(0.0, transform.getRotation().getZ(), kEpsilon));
  }

  @Test
  void testToPose2d() {
    var pose =
        new Pose3d(
            1.0,
            2.0,
            3.0,
            new Rotation3d(
                Units.degreesToRadians(20.0),
                Units.degreesToRadians(30.0),
                Units.degreesToRadians(40.0)));
    var expected = new Pose2d(1.0, 2.0, new Rotation2d(Units.degreesToRadians(40.0)));

    assertEquals(expected, pose.toPose2d());
  }
}
