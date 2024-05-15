// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class Twist3dTest {
  @Test
  void testStraightX() {
    var straight = new Twist3d(5.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    var straightPose = Pose3d.kZero.exp(straight);

    var expected = new Pose3d(5.0, 0.0, 0.0, Rotation3d.kZero);
    assertEquals(expected, straightPose);
  }

  @Test
  void testStraightY() {
    var straight = new Twist3d(0.0, 5.0, 0.0, 0.0, 0.0, 0.0);
    var straightPose = Pose3d.kZero.exp(straight);

    var expected = new Pose3d(0.0, 5.0, 0.0, Rotation3d.kZero);
    assertEquals(expected, straightPose);
  }

  @Test
  void testStraightZ() {
    var straight = new Twist3d(0.0, 0.0, 5.0, 0.0, 0.0, 0.0);
    var straightPose = Pose3d.kZero.exp(straight);

    var expected = new Pose3d(0.0, 0.0, 5.0, Rotation3d.kZero);
    assertEquals(expected, straightPose);
  }

  @Test
  void testQuarterCircle() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var quarterCircle = new Twist3d(5.0 / 2.0 * Math.PI, 0.0, 0.0, 0.0, 0.0, Math.PI / 2.0);
    var quarterCirclePose = Pose3d.kZero.exp(quarterCircle);

    var expected = new Pose3d(5.0, 5.0, 0.0, new Rotation3d(zAxis, Units.degreesToRadians(90.0)));
    assertEquals(expected, quarterCirclePose);
  }

  @Test
  void testDiagonalNoDtheta() {
    var diagonal = new Twist3d(2.0, 2.0, 0.0, 0.0, 0.0, 0.0);
    var diagonalPose = Pose3d.kZero.exp(diagonal);

    var expected = new Pose3d(2.0, 2.0, 0.0, Rotation3d.kZero);
    assertEquals(expected, diagonalPose);
  }

  @Test
  void testEquality() {
    var one = new Twist3d(5, 1, 0, 0.0, 0.0, 3.0);
    var two = new Twist3d(5, 1, 0, 0.0, 0.0, 3.0);
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = new Twist3d(5, 1, 0, 0.0, 0.0, 3.0);
    var two = new Twist3d(5, 1.2, 0, 0.0, 0.0, 3.0);
    assertNotEquals(one, two);
  }

  @Test
  void testPose3dLogX() {
    final var start = Pose3d.kZero;
    final var end =
        new Pose3d(0.0, 5.0, 5.0, new Rotation3d(Units.degreesToRadians(90.0), 0.0, 0.0));

    final var twist = start.log(end);

    var expected =
        new Twist3d(0.0, 5.0 / 2.0 * Math.PI, 0.0, Units.degreesToRadians(90.0), 0.0, 0.0);
    assertEquals(expected, twist);

    // Make sure computed twist gives back original end pose
    final var reapplied = start.exp(twist);
    assertEquals(end, reapplied);
  }

  @Test
  void testPose3dLogY() {
    final var start = Pose3d.kZero;
    final var end =
        new Pose3d(5.0, 0.0, 5.0, new Rotation3d(0.0, Units.degreesToRadians(90.0), 0.0));

    final var twist = start.log(end);

    var expected = new Twist3d(0.0, 0.0, 5.0 / 2.0 * Math.PI, 0.0, Math.PI / 2.0, 0.0);
    assertEquals(expected, twist);

    // Make sure computed twist gives back original end pose
    final var reapplied = start.exp(twist);
    assertEquals(end, reapplied);
  }

  @Test
  void testPose3dLogZ() {
    final var start = Pose3d.kZero;
    final var end =
        new Pose3d(5.0, 5.0, 0.0, new Rotation3d(0.0, 0.0, Units.degreesToRadians(90.0)));

    final var twist = start.log(end);

    var expected = new Twist3d(5.0 / 2.0 * Math.PI, 0.0, 0.0, 0.0, 0.0, Math.PI / 2.0);
    assertEquals(expected, twist);

    // Make sure computed twist gives back original end pose
    final var reapplied = start.exp(twist);
    assertEquals(end, reapplied);
  }
}
