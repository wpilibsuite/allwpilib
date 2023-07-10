// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import org.junit.jupiter.api.Test;

class ChassisSpeedsTest {
  private static final double kEpsilon = 1E-9;

  @Test
  void testVeeringCorrection() {
    final var duration = 1.0; // duration of observation
    final var dt = 0.01; // time increment for simulation
    final var target = new ChassisSpeeds(1.0, 0.0, 0.5);
    final var speeds = ChassisSpeeds.fromDiscreteSpeeds(target, duration);
    final var twist =
        new Twist2d(
            speeds.vxMetersPerSecond * dt,
            speeds.vyMetersPerSecond * dt,
            speeds.omegaRadiansPerSecond * dt);
    var pose = new Pose2d();
    for (double time = 0; time < duration; time += dt) {
      pose = pose.exp(twist);
    }
    final var result = pose; // For lambda capture
    assertAll(
        () -> assertEquals(target.vxMetersPerSecond * duration, result.getX(), kEpsilon),
        () -> assertEquals(target.vyMetersPerSecond * duration, result.getY(), kEpsilon),
        () ->
            assertEquals(
                target.omegaRadiansPerSecond * duration,
                result.getRotation().getRadians(),
                kEpsilon));
  }

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
