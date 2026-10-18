// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.wpilib.math.util.UnitConversions.degreesToRadians;
import static org.wpilib.units.Units.Inches;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.VecBuilder;

class Translation3dTest {
  private static final double EPSILON = 1E-9;

  @Test
  void testNewWithMeasures() {
    var translation = new Translation3d(Inches.of(6), Inches.of(8), Inches.of(16));

    assertEquals(0.1524, translation.x, EPSILON);
    assertEquals(0.2032, translation.y, EPSILON);
    assertEquals(0.4064, translation.z, EPSILON);
  }

  @Test
  void testSum() {
    var one = new Translation3d(1.0, 3.0, 5.0);
    var two = new Translation3d(2.0, 5.0, 8.0);

    var sum = one.plus(two);

    assertAll(
        () -> assertEquals(3.0, sum.x, EPSILON),
        () -> assertEquals(8.0, sum.y, EPSILON),
        () -> assertEquals(13.0, sum.z, EPSILON));
  }

  @Test
  void testDifference() {
    var one = new Translation3d(1.0, 3.0, 5.0);
    var two = new Translation3d(2.0, 5.0, 8.0);

    var difference = one.minus(two);

    assertAll(
        () -> assertEquals(-1.0, difference.x, EPSILON),
        () -> assertEquals(-2.0, difference.y, EPSILON),
        () -> assertEquals(-3.0, difference.z, EPSILON));
  }

  @Test
  void testRotateBy() {
    var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var translation = new Translation3d(1.0, 2.0, 3.0);

    var rotated1 = translation.rotateBy(new Rotation3d(xAxis, degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(1.0, rotated1.x, EPSILON),
        () -> assertEquals(-3.0, rotated1.y, EPSILON),
        () -> assertEquals(2.0, rotated1.z, EPSILON));

    var rotated2 = translation.rotateBy(new Rotation3d(yAxis, degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(3.0, rotated2.x, EPSILON),
        () -> assertEquals(2.0, rotated2.y, EPSILON),
        () -> assertEquals(-1.0, rotated2.z, EPSILON));

    var rotated3 = translation.rotateBy(new Rotation3d(zAxis, degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(-2.0, rotated3.x, EPSILON),
        () -> assertEquals(1.0, rotated3.y, EPSILON),
        () -> assertEquals(3.0, rotated3.z, EPSILON));
  }

  @Test
  void testRotateAround() {
    var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var translation = new Translation3d(1.0, 2.0, 3.0);
    var around = new Translation3d(3.0, 2.0, 1.0);

    var rotated1 = translation.rotateAround(around, new Rotation3d(xAxis, degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(1.0, rotated1.x, EPSILON),
        () -> assertEquals(0.0, rotated1.y, EPSILON),
        () -> assertEquals(1.0, rotated1.z, EPSILON));

    var rotated2 = translation.rotateAround(around, new Rotation3d(yAxis, degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(5.0, rotated2.x, EPSILON),
        () -> assertEquals(2.0, rotated2.y, EPSILON),
        () -> assertEquals(3.0, rotated2.z, EPSILON));

    var rotated3 = translation.rotateAround(around, new Rotation3d(zAxis, degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(3.0, rotated3.x, EPSILON),
        () -> assertEquals(0.0, rotated3.y, EPSILON),
        () -> assertEquals(3.0, rotated3.z, EPSILON));
  }

  @Test
  void testToTranslation2d() {
    var translation = new Translation3d(1.0, 2.0, 3.0);
    var expected = new Translation2d(1.0, 2.0);

    assertEquals(expected, translation.toTranslation2d());
  }

  @Test
  void testMultiplication() {
    var original = new Translation3d(3.0, 5.0, 7.0);
    var mult = original.times(3);

    assertAll(
        () -> assertEquals(9.0, mult.x, EPSILON),
        () -> assertEquals(15.0, mult.y, EPSILON),
        () -> assertEquals(21.0, mult.z, EPSILON));
  }

  @Test
  void testDivision() {
    var original = new Translation3d(3.0, 5.0, 7.0);
    var div = original.div(2);

    assertAll(
        () -> assertEquals(1.5, div.x, EPSILON),
        () -> assertEquals(2.5, div.y, EPSILON),
        () -> assertEquals(3.5, div.z, EPSILON));
  }

  @Test
  void testNorm() {
    var one = new Translation3d(3.0, 5.0, 7.0);
    assertEquals(Math.sqrt(83.0), one.getNorm(), EPSILON);
  }

  @Test
  void testSquaredNorm() {
    var one = new Translation3d(3.0, 5.0, 7.0);
    assertEquals(83.0, one.getSquaredNorm(), EPSILON);
  }

  @Test
  void testDistance() {
    var one = new Translation3d(1.0, 1.0, 1.0);
    var two = new Translation3d(6.0, 6.0, 6.0);
    assertEquals(5.0 * Math.sqrt(3.0), one.getDistance(two), EPSILON);
  }

  @Test
  void testSquaredDistance() {
    var one = new Translation3d(1.0, 1.0, 1.0);
    var two = new Translation3d(6.0, 6.0, 6.0);
    assertEquals(75.0, one.getSquaredDistance(two), EPSILON);
  }

  @Test
  void testUnaryMinus() {
    var original = new Translation3d(-4.5, 7.0, 9.0);
    var inverted = original.unaryMinus();

    assertAll(
        () -> assertEquals(4.5, inverted.x, EPSILON),
        () -> assertEquals(-7.0, inverted.y, EPSILON),
        () -> assertEquals(-9.0, inverted.z, EPSILON));
  }

  @Test
  void testEquality() {
    var one = new Translation3d(9, 5.5, 3.5);
    var two = new Translation3d(9, 5.5, 3.5);
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = new Translation3d(9, 5.5, 3.5);
    var two = new Translation3d(9, 5.7, 3.5);
    assertNotEquals(one, two);
  }

  @Test
  void testPolarConstructor() {
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var one = new Translation3d(Math.sqrt(2), new Rotation3d(zAxis, degreesToRadians(45.0)));
    var two = new Translation3d(2, new Rotation3d(zAxis, degreesToRadians(60.0)));
    assertAll(
        () -> assertEquals(1.0, one.x, EPSILON),
        () -> assertEquals(1.0, one.y, EPSILON),
        () -> assertEquals(0.0, one.z, EPSILON),
        () -> assertEquals(1.0, two.x, EPSILON),
        () -> assertEquals(Math.sqrt(3.0), two.y, EPSILON),
        () -> assertEquals(0.0, two.z, EPSILON));
  }

  @Test
  void testToVector() {
    var vec = VecBuilder.fill(1.0, 2.0, 3.0);
    var translation = new Translation3d(vec);

    assertEquals(vec.get(0), translation.x);
    assertEquals(vec.get(1), translation.y);
    assertEquals(vec.get(2), translation.z);

    assertEquals(vec, translation.toVector());
  }

  @Test
  void testNearest() {
    var origin = Translation3d.ZERO;

    // Distance sort
    // translations are in order of closest to farthest away from the origin at various positions
    // in 3D space.
    final var translation1 = new Translation3d(1, 0, 0);
    final var translation2 = new Translation3d(0, 2, 0);
    final var translation3 = new Translation3d(0, 0, 3);
    final var translation4 = new Translation3d(2, 2, 2);
    final var translation5 = new Translation3d(3, 3, 3);

    assertEquals(translation3, origin.nearest(List.of(translation5, translation3, translation4)));
    assertEquals(translation1, origin.nearest(List.of(translation1, translation2, translation3)));
    assertEquals(translation2, origin.nearest(List.of(translation4, translation2, translation3)));
  }

  @Test
  void testDot() {
    var one = new Translation3d(1.0, 2.0, 3.0);
    var two = new Translation3d(4.0, 5.0, 6.0);
    assertEquals(32.0, one.dot(two));
  }

  @Test
  void testCross() {
    var one = new Translation3d(1.0, 2.0, 3.0);
    var two = new Translation3d(4.0, 5.0, 6.0);

    var cross = one.cross(two);
    assertAll(
        () -> assertEquals(-3.0, cross.get(0, 0), EPSILON),
        () -> assertEquals(6.0, cross.get(1, 0), EPSILON),
        () -> assertEquals(-3.0, cross.get(2, 0), EPSILON));
  }
}
