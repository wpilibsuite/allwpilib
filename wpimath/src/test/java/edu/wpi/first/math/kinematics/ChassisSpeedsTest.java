// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class ChassisSpeedsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testFieldRelativeConstruction() {
    final var chassisSpeeds =
        ChassisSpeeds.fromFieldRelativeSpeeds(1.0, 0.0, 0.5, Rotation2d.fromDegrees(-90.0));

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(1.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.5, chassisSpeeds.omegaRadiansPerSecond, kEpsilon));
  }
}
