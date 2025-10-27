// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class DifferentialDriveOdometryTest {
  private static final double kEpsilon = 1E-9;
  private final DifferentialDriveOdometry m_odometry =
      new DifferentialDriveOdometry(Rotation2d.kZero, 0, 0);

  @Test
  void testOdometryWithEncoderDistances() {
    m_odometry.resetPosition(Rotation2d.fromDegrees(45), 0, 0, Pose2d.kZero);
    var pose = m_odometry.update(Rotation2d.fromDegrees(135.0), 0.0, 5 * Math.PI);

    assertAll(
        () -> assertEquals(pose.getX(), 5.0, kEpsilon),
        () -> assertEquals(pose.getY(), 5.0, kEpsilon),
        () -> assertEquals(pose.getRotation().getDegrees(), 90.0, kEpsilon));
  }
}
