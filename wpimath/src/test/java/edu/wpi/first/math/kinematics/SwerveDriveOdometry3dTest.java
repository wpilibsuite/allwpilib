// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.*;
import edu.wpi.first.math.util.Units;
import org.junit.jupiter.api.Test;

class SwerveDriveOdometry3dTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final SwerveModulePosition zero = new SwerveModulePosition();

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br);

  private final SwerveDriveOdometry3d m_odometry =
      new SwerveDriveOdometry3d(
          m_kinematics, Rotation3d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});

  @Test
  void testInitialize() {
    SwerveDriveOdometry3d odometry =
        new SwerveDriveOdometry3d(
            m_kinematics,
            Rotation3d.kZero,
            new SwerveModulePosition[] {zero, zero, zero, zero},
            new Pose3d(1, 2, 0, new Rotation3d(0, 0, Units.degreesToRadians(45))));
    var pose = odometry.getPose();
    assertAll(
        () -> assertEquals(pose.getX(), 1.0, 1e-9),
        () -> assertEquals(pose.getY(), 2.0, 1e-9),
        () -> assertEquals(pose.getZ(), 0.0, 1e-9),
        () -> assertEquals(pose.getRotation().toRotation2d().getDegrees(), 45.0, 1e-9));
  }

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
        Rotation3d.kZero,
        new SwerveModulePosition[] {
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition(),
          new SwerveModulePosition()
        });
    var pose = m_odometry.update(Rotation3d.kZero, wheelDeltas);

    assertAll(
        () -> assertEquals(5.0 / 10.0, pose.getX(), 0.01),
        () -> assertEquals(0, pose.getY(), 0.01),
        () -> assertEquals(0, pose.getZ(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().toRotation2d().getDegrees(), 0.01));
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

    m_odometry.update(Rotation3d.kZero, new SwerveModulePosition[] {zero, zero, zero, zero});
    final var pose = m_odometry.update(new Rotation3d(0, 0, Math.PI / 2), wheelDeltas);

    assertAll(
        () -> assertEquals(12.0, pose.getX(), 0.01),
        () -> assertEquals(12.0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getZ(), 0.01),
        () -> assertEquals(90.0, pose.getRotation().toRotation2d().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = new Rotation3d(0, 0, Math.PI / 2);
    var fieldAngle = Rotation3d.kZero;
    m_odometry.resetPosition(
        gyro,
        new SwerveModulePosition[] {zero, zero, zero, zero},
        new Pose3d(Translation3d.kZero, fieldAngle));
    var delta = new SwerveModulePosition();
    m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});
    delta = new SwerveModulePosition(1.0, Rotation2d.kZero);
    var pose = m_odometry.update(gyro, new SwerveModulePosition[] {delta, delta, delta, delta});

    assertAll(
        () -> assertEquals(1.0, pose.getX(), 0.1),
        () -> assertEquals(0.00, pose.getY(), 0.1),
        () -> assertEquals(0.00, pose.getZ(), 0.1),
        () -> assertEquals(0.00, pose.getRotation().toRotation2d().getRadians(), 0.1));
  }
}
