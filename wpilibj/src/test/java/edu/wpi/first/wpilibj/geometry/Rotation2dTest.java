/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class Rotation2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testRadiansAndDegrees() {
    Rotation2d one = Rotation2d.fromDegrees(45.0);
    Rotation2d two = new Rotation2d(Math.PI / 4);

    assertAll(
        () -> assertEquals(one.getRadians(), Math.PI / 4, kEpsilon),
        () -> assertEquals(two.getDegrees(), 45.0, kEpsilon)
    );
  }

  @Test
  void testRotateByFromZero() {
    Rotation2d zero = new Rotation2d();
    var rotated = zero.rotateBy(Rotation2d.fromDegrees(90.0));

    assertAll(
        () -> assertEquals(rotated.getRadians(), Math.PI / 2.0, kEpsilon),
        () -> assertEquals(rotated.getDegrees(), 90.0, kEpsilon)
    );
  }

  @Test
  void testRotateByNonZero() {
    Rotation2d rot = Rotation2d.fromDegrees(90.0);
    rot = rot.plus(Rotation2d.fromDegrees(30.0));

    assertEquals(rot.getDegrees(), 120.0, kEpsilon);
  }
}
