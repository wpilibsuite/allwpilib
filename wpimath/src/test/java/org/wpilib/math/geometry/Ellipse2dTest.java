// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class Ellipse2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testGetFocalPoints() {
    var center = new Pose2d(1, 2, new Rotation2d());
    var ellipse = new Ellipse2d(center, 5.0, 4.0);

    var pair = ellipse.getFocalPoints();
    var a = pair.getFirst();
    var b = pair.getSecond();

    assertAll(
        () -> assertEquals(new Translation2d(-2.0, 2.0), a),
        () -> assertEquals(new Translation2d(4.0, 2.0), b));
  }

  @Test
  void testIntersects() {
    var center = new Pose2d(1.0, 2.0, new Rotation2d());
    var ellipse = new Ellipse2d(center, 2.0, 1.0);

    var pointA = new Translation2d(1.0, 3.0);
    var pointB = new Translation2d(0.0, 3.0);

    assertAll(
        () -> assertTrue(ellipse.intersects(pointA)),
        () -> assertFalse(ellipse.intersects(pointB)));
  }

  @Test
  void testContains() {
    var center = new Pose2d(-1.0, -2.0, Rotation2d.fromDegrees(45.0));
    var ellipse = new Ellipse2d(center, 2.0, 1.0);

    var pointA = new Translation2d(0.0, -1.0);
    var pointB = new Translation2d(0.5, -2.0);

    assertAll(
        () -> assertTrue(ellipse.contains(pointA)), () -> assertFalse(ellipse.contains(pointB)));
  }

  @Test
  void testDistance() {
    var center = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(270.0));
    var ellipse = new Ellipse2d(center, 1.0, 2.0);

    var point1 = new Translation2d(2.5, 2.0);
    assertEquals(0.0, ellipse.getDistance(point1), kEpsilon);

    var point2 = new Translation2d(1.0, 2.0);
    assertEquals(0.0, ellipse.getDistance(point2), kEpsilon);

    var point3 = new Translation2d(1.0, 1.0);
    assertEquals(0.0, ellipse.getDistance(point3), kEpsilon);

    var point4 = new Translation2d(-1.0, 2.5);
    assertEquals(0.19210128384806818, ellipse.getDistance(point4), kEpsilon);
  }

  @Test
  void testNearest() {
    var center = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(270.0));
    var ellipse = new Ellipse2d(center, 1.0, 2.0);

    var point1 = new Translation2d(2.5, 2.0);
    var nearestPoint1 = ellipse.nearest(point1);
    assertAll(
        () -> assertEquals(2.5, nearestPoint1.getX(), kEpsilon),
        () -> assertEquals(2.0, nearestPoint1.getY(), kEpsilon));

    var point2 = new Translation2d(1.0, 2.0);
    var nearestPoint2 = ellipse.nearest(point2);
    assertAll(
        () -> assertEquals(1.0, nearestPoint2.getX(), kEpsilon),
        () -> assertEquals(2.0, nearestPoint2.getY(), kEpsilon));

    var point3 = new Translation2d(1.0, 1.0);
    var nearestPoint3 = ellipse.nearest(point3);
    assertAll(
        () -> assertEquals(1.0, nearestPoint3.getX(), kEpsilon),
        () -> assertEquals(1.0, nearestPoint3.getY(), kEpsilon));

    var point4 = new Translation2d(-1.0, 2.5);
    var nearestPoint4 = ellipse.nearest(point4);
    assertAll(
        () -> assertEquals(-0.8512799937611617, nearestPoint4.getX(), kEpsilon),
        () -> assertEquals(2.378405333174535, nearestPoint4.getY(), kEpsilon));
  }

  @Test
  void testEquals() {
    var center1 = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(90.0));
    var ellipse1 = new Ellipse2d(center1, 2.0, 3.0);

    var center2 = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(90.0));
    var ellipse2 = new Ellipse2d(center2, 2.0, 3.0);

    var center3 = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(90.0));
    var ellipse3 = new Ellipse2d(center3, 3.0, 2.0);

    assertAll(
        () -> assertEquals(ellipse1, ellipse2),
        () -> assertNotEquals(ellipse1, ellipse3),
        () -> assertNotEquals(ellipse3, ellipse2));
  }
}
