// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;

class DoubleFollowerWheelOdometryTest {
  private final double m_xWheelYPos = 1;
  private final double m_yWheelXPos = 1;

  private final DoubleFollowerWheelPositions zero = new DoubleFollowerWheelPositions();

  private final DoubleFollowerWheelOdometry m_odometry =
      new DoubleFollowerWheelOdometry(m_xWheelYPos, m_yWheelXPos, Rotation2d.kZero, zero);

  @Test
  void testMultipleConsecutiveUpdates() {
    var wheelPositions = new DoubleFollowerWheelPositions(1, 1);

    m_odometry.resetPosition(Rotation2d.kZero, wheelPositions, Pose2d.kZero);

    m_odometry.update(Rotation2d.kZero, wheelPositions);
    var secondPose = m_odometry.update(Rotation2d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(0.0, secondPose.getX(), 0.01),
        () -> assertEquals(0.0, secondPose.getY(), 0.01),
        () -> assertEquals(0.0, secondPose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testTwoIterations() {
    final var wheelPositions = new DoubleFollowerWheelPositions(0.1, 0);
    m_odometry.resetPosition(Rotation2d.kZero, new DoubleFollowerWheelPositions(), Pose2d.kZero);

    m_odometry.update(Rotation2d.kZero, new DoubleFollowerWheelPositions());
    final var pose = m_odometry.update(Rotation2d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(0.1, pose.getX(), 0.01),
        () -> assertEquals(0.0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = Rotation2d.kCCW_Pi_2;
    var fieldAngle = Rotation2d.kZero;
    m_odometry.resetPosition(
        gyro, new DoubleFollowerWheelPositions(), new Pose2d(Translation2d.kZero, fieldAngle));
    var positions = new DoubleFollowerWheelPositions(1, 0);
    m_odometry.update(gyro, new DoubleFollowerWheelPositions());
    var pose = m_odometry.update(gyro, positions);

    assertAll(
        () -> assertEquals(1, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().getRadians(), 0.1));
  }

  @Test
  void testStraightForwardsForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(0, 5, 0);

    assertAll(
        () -> assertEquals(5.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(0.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(0.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testStraightLeftForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(0, 0, 5);

    assertAll(
        () -> assertEquals(0.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(5.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(0.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testSpinInPlaceForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(5, -5, 5);

    assertAll(
        () -> assertEquals(0.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(0.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(5.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testMixedMotionForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(5, 1, -1);

    assertAll(
        () -> assertEquals(6.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(-6.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(5.0, wheelVelocities.omega, 0.1));
  }
}
