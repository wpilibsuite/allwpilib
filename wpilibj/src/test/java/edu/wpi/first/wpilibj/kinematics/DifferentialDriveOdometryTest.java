/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class DifferentialDriveOdometryTest {
  private static final double kEpsilon = 1E-9;
  private final DifferentialDriveOdometry m_odometry = new DifferentialDriveOdometry(
      new Rotation2d());

  @Test
  void testOdometryWithEncoderDistances() {
    m_odometry.resetPosition(new Pose2d(), Rotation2d.fromDegrees(45));
    var pose = m_odometry.update(Rotation2d.fromDegrees(135.0), 0.0, 5 * Math.PI);

    assertAll(
        () -> assertEquals(pose.getX(), 5.0, kEpsilon),
        () -> assertEquals(pose.getY(), 5.0, kEpsilon),
        () -> assertEquals(pose.getRotation().getDegrees(), 90.0, kEpsilon)
    );
  }
}
