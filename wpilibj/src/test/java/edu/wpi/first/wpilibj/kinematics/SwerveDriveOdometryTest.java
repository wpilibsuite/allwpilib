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
import edu.wpi.first.wpilibj.geometry.Translation2d;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SwerveDriveOdometryTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br);

  private final SwerveDriveOdometry m_odometry = new SwerveDriveOdometry(m_kinematics,
      new Rotation2d());

  @Test
  void testTwoIterations() {
    // 5 units/sec  in the x axis (forward)
    final SwerveModuleState[] wheelSpeeds = {
        new SwerveModuleState(5, Rotation2d.fromDegrees(0)),
        new SwerveModuleState(5, Rotation2d.fromDegrees(0)),
        new SwerveModuleState(5, Rotation2d.fromDegrees(0)),
        new SwerveModuleState(5, Rotation2d.fromDegrees(0))
    };

    m_odometry.updateWithTime(0.0, new Rotation2d(),
        new SwerveModuleState(), new SwerveModuleState(),
        new SwerveModuleState(), new SwerveModuleState());
    var pose = m_odometry.updateWithTime(0.10, new Rotation2d(), wheelSpeeds);

    assertAll(
        () -> assertEquals(5.0 / 10.0, pose.getX(), 0.01),
        () -> assertEquals(0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().getDegrees(), 0.01)
    );
  }

  @Test
  void test90degreeTurn() {
    // This is a 90 degree turn about the point between front left and rear left wheels
    //        Module 0: speed 18.84955592153876 angle 90.0
    //        Module 1: speed 42.14888838624436 angle 26.565051177077986
    //        Module 2: speed 18.84955592153876 angle -90.0
    //        Module 3: speed 42.14888838624436 angle -26.565051177077986

    final SwerveModuleState[] wheelSpeeds = {
        new SwerveModuleState(18.85, Rotation2d.fromDegrees(90.0)),
        new SwerveModuleState(42.15, Rotation2d.fromDegrees(26.565)),
        new SwerveModuleState(18.85, Rotation2d.fromDegrees(-90)),
        new SwerveModuleState(42.15, Rotation2d.fromDegrees(-26.565))
    };
    final var zero = new SwerveModuleState();

    m_odometry.updateWithTime(0.0, new Rotation2d(), zero, zero, zero, zero);
    final var pose = m_odometry.updateWithTime(1.0, Rotation2d.fromDegrees(90.0), wheelSpeeds);

    assertAll(
        () -> assertEquals(12.0, pose.getX(), 0.01),
        () -> assertEquals(12.0, pose.getY(), 0.01),
        () -> assertEquals(90.0, pose.getRotation().getDegrees(), 0.01)
    );
  }

  @Test
  void testGyroAngleReset() {
    var gyro = Rotation2d.fromDegrees(90.0);
    var fieldAngle = Rotation2d.fromDegrees(0.0);
    m_odometry.resetPosition(new Pose2d(new Translation2d(), fieldAngle), gyro);
    var state = new SwerveModuleState();
    m_odometry.updateWithTime(0.0, gyro, state, state, state, state);
    state = new SwerveModuleState(1.0, Rotation2d.fromDegrees(0));
    var pose = m_odometry.updateWithTime(1.0, gyro, state, state, state, state);

    assertAll(
        () -> assertEquals(1.0, pose.getX(), 0.1),
        () -> assertEquals(0.00, pose.getY(), 0.1),
        () -> assertEquals(0.00, pose.getRotation().getRadians(), 0.1)
    );
  }

}
