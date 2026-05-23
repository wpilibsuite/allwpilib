// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Translation2d;

class TripleFollowerWheelOdometryTest {
  private static final double m_xWheel1YPos = 1;
  private static final double m_xWheel2YPos = -1;
  private static final double m_yWheelXPos = 1;

  private final TripleFollowerWheelPositions zero = new TripleFollowerWheelPositions();

  private final TripleFollowerWheelOdometry m_odometry =
      new TripleFollowerWheelOdometry(
          m_xWheel1YPos, m_xWheel2YPos, m_yWheelXPos, Rotation2d.kZero, zero, Pose2d.kZero);

  @Test
  void testThrowOnInvalidXWheelSetup() {
    assertThrows(
        IllegalArgumentException.class,
        () ->
            new TripleFollowerWheelOdometry(
                0, 0, 1, Rotation2d.kZero, zero, Pose2d.kZero));
  }

  @Test
  void testMultipleConsecutiveUpdates() {
    var wheelPositions = new TripleFollowerWheelPositions(1, 1, 1);

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
    final var wheelPositions = new TripleFollowerWheelPositions(0.1, 0.1, 0);
    m_odometry.resetPosition(Rotation2d.kZero, new TripleFollowerWheelPositions(), Pose2d.kZero);

    m_odometry.update(Rotation2d.kZero, new TripleFollowerWheelPositions());
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
        gyro, new TripleFollowerWheelPositions(), new Pose2d(Translation2d.kZero, fieldAngle));
    var positions = new TripleFollowerWheelPositions(1, 1, 0);
    m_odometry.update(gyro, new TripleFollowerWheelPositions());
    var pose = m_odometry.update(gyro, positions);
    assertAll(
        () -> assertEquals(1, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().getRadians(), 0.1));
  }

  @Test
  void testStraightForwardsForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(5, 5, 0);

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
    var wheelVelocities = m_odometry.toChassisVelocities(5, -5, -5);

    assertAll(
        () -> assertEquals(0.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(0.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(-5.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testMixedMotionForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(5, 1, -1);

    assertAll(
        () -> assertEquals(3.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(1.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(-2.0, wheelVelocities.omega, 0.1));
  }
}
