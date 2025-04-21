// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import edu.wpi.first.units.Units;
import org.junit.jupiter.api.Test;

class Rotation2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testNewWithMeasures() {
    var rot = new Rotation2d(Units.Degrees.of(45));

    assertEquals(Math.PI / 4, rot.getRadians(), kEpsilon);
  }

  @Test
  void testRadiansToDegrees() {
    var rot1 = Rotation2d.fromRadians(Math.PI / 3);
    var rot2 = Rotation2d.fromRadians(Math.PI / 4);

    assertAll(
        () -> assertEquals(60.0, rot1.getDegrees(), kEpsilon),
        () -> assertEquals(45.0, rot2.getDegrees(), kEpsilon));
  }

  @Test
  void testRadiansAndDegrees() {
    var rot1 = Rotation2d.fromDegrees(45.0);
    var rot2 = Rotation2d.fromDegrees(30.0);

    assertAll(
        () -> assertEquals(Math.PI / 4.0, rot1.getRadians(), kEpsilon),
        () -> assertEquals(Math.PI / 6.0, rot2.getRadians(), kEpsilon));
  }

  @Test
  void testRotateByFromZero() {
    var zero = Rotation2d.kZero;
    var rotated = zero.rotateBy(Rotation2d.kCCW_Pi_2);

    assertAll(
        () -> assertEquals(Math.PI / 2.0, rotated.getRadians(), kEpsilon),
        () -> assertEquals(90.0, rotated.getDegrees(), kEpsilon));
  }

  @Test
  void testRotateByNonZero() {
    var rot = Rotation2d.kCCW_Pi_2;
    rot = rot.plus(Rotation2d.fromDegrees(30.0));

    assertEquals(120.0, rot.getDegrees(), kEpsilon);
  }

  @Test
  void testMinus() {
    var rot1 = Rotation2d.fromDegrees(70.0);
    var rot2 = Rotation2d.fromDegrees(30.0);

    assertEquals(40.0, rot1.minus(rot2).getDegrees(), kEpsilon);
  }

  @Test
  void testUnaryMinus() {
    var rot = Rotation2d.fromDegrees(20.0);

    assertEquals(-20.0, rot.unaryMinus().getDegrees(), kEpsilon);
  }

  @Test
  void testMultiply() {
    var rot = Rotation2d.fromDegrees(10.0);

    assertEquals(30.0, rot.times(3.0).getDegrees(), kEpsilon);
    assertEquals(410.0, rot.times(41.0).getDegrees(), kEpsilon);
  }

  @Test
  void testEquality() {
    var rot1 = Rotation2d.fromDegrees(43.0);
    var rot2 = Rotation2d.fromDegrees(43.0);
    assertEquals(rot1, rot2);

    rot1 = Rotation2d.fromDegrees(-180.0);
    rot2 = Rotation2d.fromDegrees(180.0);
    assertEquals(rot1, rot2);
  }

  @Test
  void testInequality() {
    var rot1 = Rotation2d.fromDegrees(43.0);
    var rot2 = Rotation2d.fromDegrees(43.5);
    assertNotEquals(rot1, rot2);
  }

  @Test
  void testToMatrix() {
    var before = Rotation2d.fromDegrees(20.0);
    var after = new Rotation2d(before.toMatrix());

    assertEquals(before, after);
  }

  @Test
  void testInterpolate() {
    // 50 + (70 - 50) * 0.5 = 60
    var rot1 = Rotation2d.fromDegrees(50);
    var rot2 = Rotation2d.fromDegrees(70);
    var interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(60.0, interpolated.getDegrees(), kEpsilon);

    // -160 minus half distance between 170 and -160 (15) = -175
    rot1 = Rotation2d.fromDegrees(170);
    rot2 = Rotation2d.fromDegrees(-160);
    interpolated = rot1.interpolate(rot2, 0.5);
    assertEquals(-175.0, interpolated.getDegrees(), kEpsilon);
  }
}
