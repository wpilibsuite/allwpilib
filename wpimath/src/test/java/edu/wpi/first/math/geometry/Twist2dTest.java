// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import org.junit.jupiter.api.Test;

class Twist2dTest {
  @Test
  void testStraight() {
    var straight = new Twist2d(5.0, 0.0, 0.0);
    var straightPose = Pose2d.kZero.exp(straight);

    var expected = new Pose2d(5.0, 0.0, Rotation2d.kZero);
    assertEquals(expected, straightPose);
  }

  @Test
  void testQuarterCircle() {
    var quarterCircle = new Twist2d(5.0 / 2.0 * Math.PI, 0, Math.PI / 2.0);
    var quarterCirclePose = Pose2d.kZero.exp(quarterCircle);

    var expected = new Pose2d(5.0, 5.0, Rotation2d.kCCW_Pi_2);
    assertEquals(expected, quarterCirclePose);
  }

  @Test
  void testDiagonalNoDtheta() {
    var diagonal = new Twist2d(2.0, 2.0, 0.0);
    var diagonalPose = Pose2d.kZero.exp(diagonal);

    var expected = new Pose2d(2.0, 2.0, Rotation2d.kZero);
    assertEquals(expected, diagonalPose);
  }

  @Test
  void testEquality() {
    var one = new Twist2d(5, 1, 3);
    var two = new Twist2d(5, 1, 3);
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = new Twist2d(5, 1, 3);
    var two = new Twist2d(5, 1.2, 3);
    assertNotEquals(one, two);
  }

  @Test
  void testPose2dLog() {
    final var start = Pose2d.kZero;
    final var end = new Pose2d(5.0, 5.0, Rotation2d.kCCW_Pi_2);

    final var twist = start.log(end);

    var expected = new Twist2d(5.0 / 2.0 * Math.PI, 0.0, Math.PI / 2.0);
    assertEquals(expected, twist);

    // Make sure computed twist gives back original end pose
    final var reapplied = start.exp(twist);
    assertEquals(end, reapplied);
  }
}
