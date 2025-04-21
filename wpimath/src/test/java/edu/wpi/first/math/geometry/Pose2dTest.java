// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.units.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class Pose2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testNewWithMeasures() {
    var pose = new Pose2d(Units.Inches.of(6), Units.Inches.of(8), Rotation2d.fromDegrees(45));

    assertEquals(0.1524, pose.getX(), kEpsilon);
    assertEquals(0.2032, pose.getY(), kEpsilon);
    assertEquals(Math.PI / 4, pose.getRotation().getRadians(), kEpsilon);
  }

  @Test
  void testRotateBy() {
    final double x = 1.0;
    final double y = 2.0;
    var initial = new Pose2d(new Translation2d(x, y), Rotation2d.fromDegrees(45.0));

    var rotation = Rotation2d.fromDegrees(5.0);
    var rotated = initial.rotateBy(rotation);

    // Translation is rotated by CCW rotation matrix
    double c = rotation.getCos();
    double s = rotation.getSin();
    assertAll(
        () -> assertEquals(c * x - s * y, rotated.getX(), kEpsilon),
        () -> assertEquals(s * x + c * y, rotated.getY(), kEpsilon),
        () ->
            assertEquals(
                initial.getRotation().getDegrees() + rotation.getDegrees(),
                rotated.getRotation().getDegrees(),
                kEpsilon));
  }

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
  void testRotateAround() {
    var initial = new Pose2d(5, 0, Rotation2d.kZero);
    var point = Translation2d.kZero;

    var rotated = initial.rotateAround(point, Rotation2d.kPi);

    assertAll(
        () -> assertEquals(-5.0, rotated.getX(), kEpsilon),
        () -> assertEquals(0.0, rotated.getY(), kEpsilon),
        () -> assertEquals(180.0, rotated.getRotation().getDegrees(), kEpsilon));
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
  void testToMatrix() {
    var before = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(20.0));
    var after = new Pose2d(before.toMatrix());

    assertEquals(before, after);
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

  @Test
  void testNearest() {
    var origin = Pose2d.kZero;

    // Distance sort
    // each poseX is X units away from the origin at a random angle.
    final var pose1 =
        new Pose2d(new Translation2d(1, Rotation2d.fromDegrees(45)), Rotation2d.kZero);
    final var pose2 = new Pose2d(new Translation2d(2, Rotation2d.kCCW_Pi_2), Rotation2d.kZero);
    final var pose3 =
        new Pose2d(new Translation2d(3, Rotation2d.fromDegrees(135)), Rotation2d.kZero);
    final var pose4 = new Pose2d(new Translation2d(4, Rotation2d.kPi), Rotation2d.kZero);
    final var pose5 = new Pose2d(new Translation2d(5, Rotation2d.kCW_Pi_2), Rotation2d.kZero);

    assertEquals(pose3, origin.nearest(List.of(pose5, pose3, pose4)));
    assertEquals(pose1, origin.nearest(List.of(pose1, pose2, pose3)));
    assertEquals(pose2, origin.nearest(List.of(pose4, pose2, pose3)));

    // Rotation component sort (when distance is the same)
    // Use the same translation because using different angles at the same distance can cause
    // rounding error.
    final var translation = new Translation2d(1, Rotation2d.kZero);

    final var poseA = new Pose2d(translation, Rotation2d.kZero);
    final var poseB = new Pose2d(translation, Rotation2d.fromDegrees(30));
    final var poseC = new Pose2d(translation, Rotation2d.fromDegrees(120));
    final var poseD = new Pose2d(translation, Rotation2d.kCCW_Pi_2);
    final var poseE = new Pose2d(translation, Rotation2d.fromDegrees(-180));

    assertEquals(
        poseA, new Pose2d(0, 0, Rotation2d.fromDegrees(360)).nearest(List.of(poseA, poseB, poseD)));
    assertEquals(
        poseB,
        new Pose2d(0, 0, Rotation2d.fromDegrees(-335)).nearest(List.of(poseB, poseC, poseD)));
    assertEquals(
        poseC,
        new Pose2d(0, 0, Rotation2d.fromDegrees(-120)).nearest(List.of(poseB, poseC, poseD)));
    assertEquals(
        poseD, new Pose2d(0, 0, Rotation2d.fromDegrees(85)).nearest(List.of(poseA, poseC, poseD)));
    assertEquals(
        poseE, new Pose2d(0, 0, Rotation2d.fromDegrees(170)).nearest(List.of(poseA, poseD, poseE)));
  }
}
