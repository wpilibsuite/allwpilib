// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.util.Units;

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
    var pose = odometry.getPose();
    assertAll(
        () -> assertEquals(1.0, pose.getX(), kEpsilon),
        () -> assertEquals(2.0, pose.getY(), kEpsilon),
        () -> assertEquals(0.0, pose.getZ(), kEpsilon),
        () -> assertEquals(45.0, pose.getRotation().toRotation2d().getDegrees(), kEpsilon));
  }

  @Test
  void testOdometryWithEncoderDistances() {
    m_odometry.resetPosition(new Rotation3d(0, 0, Units.degreesToRadians(45)), 0, 0, Pose3d.kZero);
    var pose =
        m_odometry.update(new Rotation3d(0, 0, Units.degreesToRadians(135.0)), 0.0, 5 * Math.PI);

    assertAll(
        () -> assertEquals(5.0, pose.getX(), kEpsilon),
        () -> assertEquals(5.0, pose.getY(), kEpsilon),
        () -> assertEquals(0.0, pose.getZ(), kEpsilon),
        () -> assertEquals(90.0, pose.getRotation().toRotation2d().getDegrees(), kEpsilon));
  }

  @Test
  void testGyroOffset() {
    m_odometry.resetPosition(
        new Rotation3d(0, Units.degreesToRadians(5), 0),
        0,
        0,
        new Pose3d(Translation3d.kZero, new Rotation3d(0, 0, Units.degreesToRadians(90))));
    var pose = m_odometry.update(new Rotation3d(0, Units.degreesToRadians(10), 0), 0, 0);

    assertAll(
        () -> assertEquals(0.0, pose.getX(), kEpsilon),
        () -> assertEquals(0.0, pose.getY(), kEpsilon),
        () -> assertEquals(0.0, pose.getZ(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(0), pose.getRotation().getX(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(5), pose.getRotation().getY(), kEpsilon),
        () -> assertEquals(Units.degreesToRadians(90), pose.getRotation().getZ(), kEpsilon));
  }
}
