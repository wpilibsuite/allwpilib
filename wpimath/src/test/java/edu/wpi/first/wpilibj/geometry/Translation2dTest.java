/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

class Translation2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testSum() {
    var one = new Translation2d(1.0, 3.0);
    var two = new Translation2d(2.0, 5.0);

    var sum = one.plus(two);

    assertAll(
        () -> assertEquals(sum.getX(), 3.0, kEpsilon),
        () -> assertEquals(sum.getY(), 8.0, kEpsilon)
    );
  }

  @Test
  void testDifference() {
    var one = new Translation2d(1.0, 3.0);
    var two = new Translation2d(2.0, 5.0);

    var difference = one.minus(two);

    assertAll(
        () -> assertEquals(difference.getX(), -1.0, kEpsilon),
        () -> assertEquals(difference.getY(), -2.0, kEpsilon)
    );
  }

  @Test
  void testRotateBy() {
    var another = new Translation2d(3.0, 0.0);
    var rotated = another.rotateBy(Rotation2d.fromDegrees(90.0));

    assertAll(
        () -> assertEquals(rotated.getX(), 0.0, kEpsilon),
        () -> assertEquals(rotated.getY(), 3.0, kEpsilon)
    );
  }

  @Test
  void testMultiplication() {
    var original = new Translation2d(3.0, 5.0);
    var mult = original.times(3);

    assertAll(
        () -> assertEquals(mult.getX(), 9.0, kEpsilon),
        () -> assertEquals(mult.getY(), 15.0, kEpsilon)
    );
  }

  @Test
  void testDivision() {
    var original = new Translation2d(3.0, 5.0);
    var div = original.div(2);

    assertAll(
        () -> assertEquals(div.getX(), 1.5, kEpsilon),
        () -> assertEquals(div.getY(), 2.5, kEpsilon)
    );
  }

  @Test
  void testNorm() {
    var one = new Translation2d(3.0, 5.0);
    assertEquals(one.getNorm(), Math.hypot(3.0, 5.0), kEpsilon);
  }

  @Test
  void testDistance() {
    var one = new Translation2d(1, 1);
    var two = new Translation2d(6, 6);
    assertEquals(one.getDistance(two), 5 * Math.sqrt(2), kEpsilon);
  }

  @Test
  void testUnaryMinus() {
    var original = new Translation2d(-4.5, 7);
    var inverted = original.unaryMinus();

    assertAll(
        () -> assertEquals(inverted.getX(), 4.5, kEpsilon),
        () -> assertEquals(inverted.getY(), -7, kEpsilon)
    );
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
        () -> assertEquals(one.getX(), 1.0, kEpsilon),
        () -> assertEquals(one.getY(), 1.0, kEpsilon),
        () -> assertEquals(two.getX(), 1.0, kEpsilon),
        () -> assertEquals(two.getY(), Math.sqrt(3), kEpsilon)
    );
  }
}
