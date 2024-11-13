// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class DifferentialDriveOdometry3dTest {
  private static final double kEpsilon = 1E-9;
  private final DifferentialDriveOdometry3d m_odometry =
      new DifferentialDriveOdometry3d(Rotation3d.kZero, 0, 0);

  @Test
  void testInitialize() {
    DifferentialDriveOdometry3d odometry =
        new DifferentialDriveOdometry3d(
            Rotation3d.kZero,
            0,
            0,
            new Pose3d(1, 2, 0, new Rotation3d(0, 0, Units.degreesToRadians(45))));
    var pose = odometry.getPoseMeters();
    assertAll(
        () -> assertEquals(pose.getX(), 1.0, kEpsilon),
        () -> assertEquals(pose.getY(), 2.0, kEpsilon),
        () -> assertEquals(pose.getZ(), 0.0, kEpsilon),
        () -> assertEquals(pose.getRotation().toRotation2d().getDegrees(), 45.0, kEpsilon));
  }

  @Test
  void testOdometryWithEncoderDistances() {
    m_odometry.resetPosition(new Rotation3d(0, 0, Units.degreesToRadians(45)), 0, 0, Pose3d.kZero);
    var pose =
        m_odometry.update(new Rotation3d(0, 0, Units.degreesToRadians(135.0)), 0.0, 5 * Math.PI);

    assertAll(
        () -> assertEquals(pose.getX(), 5.0, kEpsilon),
        () -> assertEquals(pose.getY(), 5.0, kEpsilon),
        () -> assertEquals(pose.getZ(), 0.0, kEpsilon),
        () -> assertEquals(pose.getRotation().toRotation2d().getDegrees(), 90.0, kEpsilon));
  }
}
