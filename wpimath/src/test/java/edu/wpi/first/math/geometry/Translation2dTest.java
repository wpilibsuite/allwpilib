// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.math.VecBuilder;
import java.util.List;
import org.junit.jupiter.api.Test;

class Translation2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testSum() {
    var one = new Translation2d(1.0, 3.0);
    var two = new Translation2d(2.0, 5.0);

    var sum = one.plus(two);

    assertAll(
        () -> assertEquals(3.0, sum.getX(), kEpsilon),
        () -> assertEquals(8.0, sum.getY(), kEpsilon));
  }

  @Test
  void testDifference() {
    var one = new Translation2d(1.0, 3.0);
    var two = new Translation2d(2.0, 5.0);

    var difference = one.minus(two);

    assertAll(
        () -> assertEquals(-1.0, difference.getX(), kEpsilon),
        () -> assertEquals(-2.0, difference.getY(), kEpsilon));
  }

  @Test
  void testRotateBy() {
    var another = new Translation2d(3.0, 0.0);
    var rotated = another.rotateBy(Rotation2d.kCCW_Pi_2);

    assertAll(
        () -> assertEquals(0.0, rotated.getX(), kEpsilon),
        () -> assertEquals(3.0, rotated.getY(), kEpsilon));
  }

  @Test
  void testRotateAround() {
    var original = new Translation2d(2.0, 1.0);
    var other = new Translation2d(3.0, 2.0);
    var rotated = original.rotateAround(other, Rotation2d.fromDegrees(180.0));

    assertAll(
        () -> assertEquals(4.0, rotated.getX(), kEpsilon),
        () -> assertEquals(3.0, rotated.getY(), kEpsilon));
  }

  @Test
  void testMultiplication() {
    var original = new Translation2d(3.0, 5.0);
    var mult = original.times(3);

    assertAll(
        () -> assertEquals(9.0, mult.getX(), kEpsilon),
        () -> assertEquals(15.0, mult.getY(), kEpsilon));
  }

  @Test
  void testDivision() {
    var original = new Translation2d(3.0, 5.0);
    var div = original.div(2);

    assertAll(
        () -> assertEquals(1.5, div.getX(), kEpsilon),
        () -> assertEquals(2.5, div.getY(), kEpsilon));
  }

  @Test
  void testNorm() {
    var one = new Translation2d(3.0, 5.0);
    assertEquals(Math.hypot(3.0, 5.0), one.getNorm(), kEpsilon);
  }

  @Test
  void testDistance() {
    var one = new Translation2d(1, 1);
    var two = new Translation2d(6, 6);
    assertEquals(5.0 * Math.sqrt(2.0), one.getDistance(two), kEpsilon);
  }

  @Test
  void testUnaryMinus() {
    var original = new Translation2d(-4.5, 7);
    var inverted = original.unaryMinus();

    assertAll(
        () -> assertEquals(4.5, inverted.getX(), kEpsilon),
        () -> assertEquals(-7.0, inverted.getY(), kEpsilon));
  }

  @Test
  void testEquality() {
    var one = new Translation2d(9, 5.5);
    var two = new Translation2d(9, 5.5);
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = new Translation2d(9, 5.5);
    var two = new Translation2d(9, 5.7);
    assertNotEquals(one, two);
  }

  @Test
  void testPolarConstructor() {
    var one = new Translation2d(Math.sqrt(2), Rotation2d.fromDegrees(45.0));
    var two = new Translation2d(2, Rotation2d.fromDegrees(60.0));
    assertAll(
        () -> assertEquals(1.0, one.getX(), kEpsilon),
        () -> assertEquals(1.0, one.getY(), kEpsilon),
        () -> assertEquals(1.0, two.getX(), kEpsilon),
        () -> assertEquals(Math.sqrt(3.0), two.getY(), kEpsilon));
  }

  @Test
  void testNearest() {
    var origin = Translation2d.kZero;

    // each translationX is X units away from the origin at a random angle.
    var translation1 = new Translation2d(1, Rotation2d.fromDegrees(45));
    var translation2 = new Translation2d(2, Rotation2d.kCCW_Pi_2);
    var translation3 = new Translation2d(3, Rotation2d.fromDegrees(135));
    var translation4 = new Translation2d(4, Rotation2d.kPi);
    var translation5 = new Translation2d(5, Rotation2d.kCW_Pi_2);

    assertEquals(origin.nearest(List.of(translation5, translation3, translation4)), translation3);
    assertEquals(origin.nearest(List.of(translation1, translation2, translation3)), translation1);
    assertEquals(origin.nearest(List.of(translation4, translation2, translation3)), translation2);
  }

  @Test
  void testToVector() {
    var vec = VecBuilder.fill(1.0, 2.0);
    var translation = new Translation2d(vec);

    assertEquals(vec.get(0), translation.getX());
    assertEquals(vec.get(1), translation.getY());

    assertEquals(vec, translation.toVector());
  }
}
