/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.geometry;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class Transform2dTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testInverse() {
    var initial = new Pose2d(new Translation2d(1.0, 2.0), Rotation2d.fromDegrees(45.0));
    var transformation = new Transform2d(new Translation2d(5.0, 0.0),
        Rotation2d.fromDegrees(5.0));

    var transformed = initial.plus(transformation);
    var untransformed = transformed.plus(transformation.inverse());

    assertAll(
        () -> assertEquals(initial.getX(), untransformed.getX(),
                           kEpsilon),
        () -> assertEquals(initial.getY(), untransformed.getY(),
                           kEpsilon),
        () -> assertEquals(initial.getRotation().getDegrees(),
                           untransformed.getRotation().getDegrees(), kEpsilon)
    );
  }
}
