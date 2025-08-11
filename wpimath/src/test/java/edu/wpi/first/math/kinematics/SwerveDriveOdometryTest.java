// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import org.junit.jupiter.api.Test;

class SwerveDriveOdometryTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final SwerveModulePosition zero = new SwerveModulePosition();

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br);

  private final SwerveDriveOdometry m_odometry =
      new SwerveDriveOdometry(
          m_kinematics, Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});

  @Test
  void testTwoIterations() {
    // 5 units/sec  in the x-axis (forward)
    final SwerveModulePosition[] wheelDeltas = {
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero),
      new SwerveModulePosition(0.5, Rotation2d.kZero)
    };

    m_odometry.update(
        Rotation2d.kZero,
        new SwerveModulePosition[] {
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition()
        });
    var pose = m_odometry.update(Rotation2d.kZero, wheelDeltas);

    assertAll(
        () -> assertEquals(5.0 / 10.0, pose.getX(), 0.01),
        () -> assertEquals(0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void test90degreeTurn() {
    // This is a 90 degree turn about the point between front left and rear left wheels
    //        Module 0: speed 18.84955592153876 angle 90.0
    //        Module 1: speed 42.14888838624436 angle 26.565051177077986
    //        Module 2: speed 18.84955592153876 angle -90.0
    //        Module 3: speed 42.14888838624436 angle -26.565051177077986

    final SwerveModulePosition[] wheelDeltas = {
      new SwerveModulePosition(18.85, Rotation2d.kCCW_Pi_2),
      new SwerveModulePosition(42.15, Rotation2d.fromDegrees(26.565)),
      new SwerveModulePosition(18.85, Rotation2d.kCW_Pi_2),
      new SwerveModulePosition(42.15, Rotation2d.fromDegrees(-26.565))
    };
    final var zero = new SwerveModulePosition();

    m_odometry.update(Rotation2d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});
    final var pose = m_odometry.update(Rotation2d.kCCW_Pi_2, wheelDeltas);

    assertAll(
        () -> assertEquals(12.0, pose.getX(), 0.01),
        () -> assertEquals(12.0, pose.getY(), 0.01),
        () -> assertEquals(90.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = Rotation2d.kCCW_Pi_2;
    var fieldAngle = Rotation2d.kZero;
    m_odometry.resetPosition(
        gyro,
        new SwerveModulePosition[] {zero, zero, zero, zero},
        new Pose2d(Translation2d.kZero, fieldAngle));
    var delta = new SwerveModulePosition();
    m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});
    delta = new SwerveModulePosition(1.0, Rotation2d.kZero);
    var pose = m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});

    assertAll(
        () -> assertEquals(1.0, pose.getX(), 0.1),
        () -> assertEquals(0.00, pose.getY(), 0.1),
        () -> assertEquals(0.00, pose.getRotation().getRadians(), 0.1));
  }
}
