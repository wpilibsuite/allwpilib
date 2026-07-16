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

class TwoDeadWheelOdometryTest {
  private static final double m_xWheelYPos = 1;
  private static final double m_yWheelXPos = 1;

  private final TwoDeadWheelPositions zero = new TwoDeadWheelPositions();

  private final TwoDeadWheelOdometry m_odometry =
      new TwoDeadWheelOdometry(m_xWheelYPos, m_yWheelXPos, Rotation2d.kZero, zero);

  @Test
  void testMultipleConsecutiveUpdates() {
    var wheelPositions = new TwoDeadWheelPositions(1, 1);

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
    final var wheelPositions = new TwoDeadWheelPositions(0.1, 0);
    m_odometry.resetPosition(Rotation2d.kZero, new TwoDeadWheelPositions(), Pose2d.kZero);

    m_odometry.update(Rotation2d.kZero, new TwoDeadWheelPositions());
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
        gyro, new TwoDeadWheelPositions(), new Pose2d(Translation2d.kZero, fieldAngle));
    var positions = new TwoDeadWheelPositions(1, 0);
    m_odometry.update(gyro, new TwoDeadWheelPositions());
    var pose = m_odometry.update(gyro, positions);

    assertAll(
        () -> assertEquals(1, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().getRadians(), 0.1));
  }

  @Test
  void testStraightForwardsForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(5, 0, 0);

    assertAll(
        () -> assertEquals(5.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(0.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(0.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testStraightLeftForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(0, 5, 0);

    assertAll(
        () -> assertEquals(0.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(5.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(0.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testSpinInPlaceForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(-5, 5, 5);

    assertAll(
        () -> assertEquals(0.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(0.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(5.0, wheelVelocities.omega, 0.1));
  }

  @Test
  void testMixedMotionForwardKinematics() {
    var wheelVelocities = m_odometry.toChassisVelocities(1, -1, 5);

    assertAll(
        () -> assertEquals(6.0, wheelVelocities.vx, 0.1),
        () -> assertEquals(-6.0, wheelVelocities.vy, 0.1),
        () -> assertEquals(5.0, wheelVelocities.omega, 0.1));
  }
}
