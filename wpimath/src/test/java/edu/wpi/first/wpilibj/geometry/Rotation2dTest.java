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

class Rotation2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testRadiansToDegrees() {
    var one = new Rotation2d(Math.PI / 3);
    var two = new Rotation2d(Math.PI / 4);

    assertAll(
        () -> assertEquals(one.getDegrees(), 60.0, kEpsilon),
        () -> assertEquals(two.getDegrees(), 45.0, kEpsilon)
    );
  }

  @Test
  void testRadiansAndDegrees() {
    var one = Rotation2d.fromDegrees(45.0);
    var two = Rotation2d.fromDegrees(30.0);

    assertAll(
        () -> assertEquals(one.getRadians(), Math.PI / 4, kEpsilon),
        () -> assertEquals(two.getRadians(), Math.PI / 6, kEpsilon)
    );
  }

  @Test
  void testRotateByFromZero() {
    var zero = new Rotation2d();
    var rotated = zero.rotateBy(Rotation2d.fromDegrees(90.0));

    assertAll(
        () -> assertEquals(rotated.getRadians(), Math.PI / 2.0, kEpsilon),
        () -> assertEquals(rotated.getDegrees(), 90.0, kEpsilon)
    );
  }

  @Test
  void testRotateByNonZero() {
    var rot = Rotation2d.fromDegrees(90.0);
    rot = rot.plus(Rotation2d.fromDegrees(30.0));

    assertEquals(rot.getDegrees(), 120.0, kEpsilon);
  }

  @Test
  void testMinus() {
    var one = Rotation2d.fromDegrees(70.0);
    var two = Rotation2d.fromDegrees(30.0);

    assertEquals(one.minus(two).getDegrees(), 40.0, kEpsilon);
  }

  @Test
  void testEquality() {
    var one = Rotation2d.fromDegrees(43.0);
    var two = Rotation2d.fromDegrees(43.0);
    assertEquals(one, two);
  }

  @Test
  void testInequality() {
    var one = Rotation2d.fromDegrees(43.0);
    var two = Rotation2d.fromDegrees(43.5);
    assertNotEquals(one, two);
  }
}
