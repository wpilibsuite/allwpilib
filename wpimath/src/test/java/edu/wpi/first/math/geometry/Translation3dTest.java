// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static edu.wpi.first.units.Units.Inches;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class Translation3dTest {
  private static final double EPSILON = 1E-9;

  @Test
  void testNewWithMeasures() {
    var translation = new Translation3d(Inches.of(6), Inches.of(8), Inches.of(16));

    assertEquals(0.1524, translation.getX(), EPSILON);
    assertEquals(0.2032, translation.getY(), EPSILON);
    assertEquals(0.4064, translation.getZ(), EPSILON);
  }

  @Test
  void testSum() {
    var one = new Translation3d(1.0, 3.0, 5.0);
    var two = new Translation3d(2.0, 5.0, 8.0);

    var sum = one.plus(two);

    assertAll(
        () -> assertEquals(3.0, sum.getX(), EPSILON),
        () -> assertEquals(8.0, sum.getY(), EPSILON),
        () -> assertEquals(13.0, sum.getZ(), EPSILON));
  }

  @Test
  void testDifference() {
    var one = new Translation3d(1.0, 3.0, 5.0);
    var two = new Translation3d(2.0, 5.0, 8.0);

    var difference = one.minus(two);

    assertAll(
        () -> assertEquals(-1.0, difference.getX(), EPSILON),
        () -> assertEquals(-2.0, difference.getY(), EPSILON),
        () -> assertEquals(-3.0, difference.getZ(), EPSILON));
  }

  @Test
  void testRotateBy() {
    var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var translation = new Translation3d(1.0, 2.0, 3.0);

    var rotated1 = translation.rotateBy(new Rotation3d(xAxis, Units.degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(1.0, rotated1.getX(), EPSILON),
        () -> assertEquals(-3.0, rotated1.getY(), EPSILON),
        () -> assertEquals(2.0, rotated1.getZ(), EPSILON));

    var rotated2 = translation.rotateBy(new Rotation3d(yAxis, Units.degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(3.0, rotated2.getX(), EPSILON),
        () -> assertEquals(2.0, rotated2.getY(), EPSILON),
        () -> assertEquals(-1.0, rotated2.getZ(), EPSILON));

    var rotated3 = translation.rotateBy(new Rotation3d(zAxis, Units.degreesToRadians(90.0)));
    assertAll(
        () -> assertEquals(-2.0, rotated3.getX(), EPSILON),
        () -> assertEquals(1.0, rotated3.getY(), EPSILON),
        () -> assertEquals(3.0, rotated3.getZ(), EPSILON));
  }

  @Test
  void testRotateAround() {
    var xAxis = VecBuilder.fill(1.0, 0.0, 0.0);
    var yAxis = VecBuilder.fill(0.0, 1.0, 0.0);
    var zAxis = VecBuilder.fill(0.0, 0.0, 1.0);

    var translation = new Translation3d(1.0, 2.0, 3.0);
    var around = new Translation3d(3.0, 2.0, 1.0);

    var rotated1 =
        translation.rotateAround(around, new Rotation3d(xAxis, Units.degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(1.0, rotated1.getX(), EPSILON),
        () -> assertEquals(0.0, rotated1.getY(), EPSILON),
        () -> assertEquals(1.0, rotated1.getZ(), EPSILON));

    var rotated2 =
        translation.rotateAround(around, new Rotation3d(yAxis, Units.degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(5.0, rotated2.getX(), EPSILON),
        () -> assertEquals(2.0, rotated2.getY(), EPSILON),
        () -> assertEquals(3.0, rotated2.getZ(), EPSILON));

    var rotated3 =
        translation.rotateAround(around, new Rotation3d(zAxis, Units.degreesToRadians(90.0)));

    assertAll(
        () -> assertEquals(3.0, rotated3.getX(), EPSILON),
        () -> assertEquals(0.0, rotated3.getY(), EPSILON),
        () -> assertEquals(3.0, rotated3.getZ(), EPSILON));
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
        () -> assertEquals(9.0, mult.getX(), EPSILON),
        () -> assertEquals(15.0, mult.getY(), EPSILON),
        () -> assertEquals(21.0, mult.getZ(), EPSILON));
  }

  @Test
  void testDivision() {
    var original = new Translation3d(3.0, 5.0, 7.0);
    var div = original.div(2);

    assertAll(
        () -> assertEquals(1.5, div.getX(), EPSILON),
        () -> assertEquals(2.5, div.getY(), EPSILON),
        () -> assertEquals(3.5, div.getZ(), EPSILON));
  }

  @Test
  void testNorm() {
    var one = new Translation3d(3.0, 5.0, 7.0);
    assertEquals(Math.sqrt(83.0), one.getNorm(), EPSILON);
  }

  @Test
  void testDistance() {
    var one = new Translation3d(1.0, 1.0, 1.0);
    var two = new Translation3d(6.0, 6.0, 6.0);
    assertEquals(5.0 * Math.sqrt(3.0), one.getDistance(two), EPSILON);
  }

  @Test
  void testUnaryMinus() {
    var original = new Translation3d(-4.5, 7.0, 9.0);
    var inverted = original.unaryMinus();

    assertAll(
        () -> assertEquals(4.5, inverted.getX(), EPSILON),
        () -> assertEquals(-7.0, inverted.getY(), EPSILON),
        () -> assertEquals(-9.0, inverted.getZ(), EPSILON));
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

    var one = new Translation3d(Math.sqrt(2), new Rotation3d(zAxis, Units.degreesToRadians(45.0)));
    var two = new Translation3d(2, new Rotation3d(zAxis, Units.degreesToRadians(60.0)));
    assertAll(
        () -> assertEquals(1.0, one.getX(), EPSILON),
        () -> assertEquals(1.0, one.getY(), EPSILON),
        () -> assertEquals(0.0, one.getZ(), EPSILON),
        () -> assertEquals(1.0, two.getX(), EPSILON),
        () -> assertEquals(Math.sqrt(3.0), two.getY(), EPSILON),
        () -> assertEquals(0.0, two.getZ(), EPSILON));
  }

  @Test
  void testToVector() {
    var vec = VecBuilder.fill(1.0, 2.0, 3.0);
    var translation = new Translation3d(vec);

    assertEquals(vec.get(0), translation.getX());
    assertEquals(vec.get(1), translation.getY());
    assertEquals(vec.get(2), translation.getZ());

    assertEquals(vec, translation.toVector());
  }
}
