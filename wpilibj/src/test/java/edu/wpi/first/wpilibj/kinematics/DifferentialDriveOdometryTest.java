/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
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
  private final DifferentialDriveKinematics m_kinematics
      = new DifferentialDriveKinematics(0.381 * 2);
  private final DifferentialDriveOdometry m_odometry = new DifferentialDriveOdometry(m_kinematics);

  @Test
  void testOneIteration() {
    m_odometry.resetPosition(new Pose2d());
    var speeds = new DifferentialDriveWheelSpeeds(0.02, 0.02);
    m_odometry.updateWithTime(0.0, new Rotation2d(), new DifferentialDriveWheelSpeeds());
    var pose = m_odometry.updateWithTime(1.0, new Rotation2d(), speeds);

    assertAll(
        () -> assertEquals(0.02, pose.getTranslation().getX(), kEpsilon),
        () -> assertEquals(0.00, pose.getTranslation().getY(), kEpsilon),
        () -> assertEquals(0.00, pose.getRotation().getRadians(), kEpsilon)
    );
  }

  @Test
  void testQuarterCircle() {
    m_odometry.resetPosition(new Pose2d());
    var speeds = new DifferentialDriveWheelSpeeds(0.0, 5 * Math.PI);
    m_odometry.updateWithTime(0.0, new Rotation2d(), new DifferentialDriveWheelSpeeds());
    var pose = m_odometry.updateWithTime(1.0, Rotation2d.fromDegrees(90.0), speeds);

    assertAll(
        () -> assertEquals(pose.getTranslation().getX(), 5.0, kEpsilon),
        () -> assertEquals(pose.getTranslation().getY(), 5.0, kEpsilon),
        () -> assertEquals(pose.getRotation().getDegrees(), 90.0, kEpsilon)
    );
  }
}
