// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.util.Units;

class ThreeDeadWheelOdometry3dTest {
  private static final double m_xWheel1YPos = 1;
  private static final double m_xWheel2YPos = -1;
  private static final double m_yWheelXPos = 1;

  private final ThreeDeadWheelPositions zero = new ThreeDeadWheelPositions();

  private final ThreeDeadWheelOdometry3d m_odometry =
      new ThreeDeadWheelOdometry3d(
          m_xWheel1YPos, m_xWheel2YPos, m_yWheelXPos, Rotation3d.kZero, zero, Pose3d.kZero);

  @Test
  void testThrowOnInvalidXWheelSetup() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new ThreeDeadWheelOdometry3d(0, 0, 1, Rotation3d.kZero, zero, Pose3d.kZero));
  }

  @Test
  void testMultipleConsecutiveUpdates() {
    var wheelPositions = new ThreeDeadWheelPositions(1, 1, 1);

    m_odometry.resetPosition(Rotation3d.kZero, wheelPositions, Pose3d.kZero);

    m_odometry.update(Rotation3d.kZero, wheelPositions);
    var secondPose = m_odometry.update(Rotation3d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(0.0, secondPose.getX(), 0.01),
        () -> assertEquals(0.0, secondPose.getY(), 0.01),
        () -> assertEquals(0.0, secondPose.getRotation().toRotation2d().getDegrees(), 0.01));
  }

  @Test
  void testTwoIterations() {
    final var wheelPositions = new ThreeDeadWheelPositions(0.1, 0.1, 0);
    m_odometry.resetPosition(Rotation3d.kZero, new ThreeDeadWheelPositions(), Pose3d.kZero);

    m_odometry.update(Rotation3d.kZero, new ThreeDeadWheelPositions());
    final var pose = m_odometry.update(Rotation3d.kZero, wheelPositions);

    assertAll(
        () -> assertEquals(0.1, pose.getX(), 0.01),
        () -> assertEquals(0.0, pose.getY(), 0.01),
        () -> assertEquals(0.0, pose.getRotation().toRotation2d().getDegrees(), 0.01));
  }

  @Test
  void testGyroAngleReset() {
    var gyro = new Rotation3d(0, 0, Math.PI / 2);
    var fieldAngle = Rotation3d.kZero;
    m_odometry.resetPosition(
        gyro, new ThreeDeadWheelPositions(), new Pose3d(Translation3d.kZero, fieldAngle));
    var positions = new ThreeDeadWheelPositions(1, 1, 0);
    m_odometry.update(gyro, new ThreeDeadWheelPositions());
    var pose = m_odometry.update(gyro, positions);
    assertAll(
        () -> assertEquals(1, pose.getX(), 0.1),
        () -> assertEquals(0.0, pose.getY(), 0.1),
        () -> assertEquals(0.0, pose.getRotation().toRotation2d().getRadians(), 0.1));
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

  @Test
  void testGyroOffset() {
    var wheelPositions = new ThreeDeadWheelPositions();
    m_odometry.resetPosition(
        new Rotation3d(0, Units.degreesToRadians(5), 0),
        wheelPositions,
        new Pose3d(Translation3d.kZero, new Rotation3d(0, 0, Units.degreesToRadians(90))));
    var pose = m_odometry.update(new Rotation3d(0, Units.degreesToRadians(10), 0), wheelPositions);
    assertAll(
        () -> assertEquals(0.0, pose.getX(), 1e-9),
        () -> assertEquals(0.0, pose.getY(), 1e-9),
        () -> assertEquals(0.0, pose.getZ(), 1e-9),
        () -> assertEquals(Units.degreesToRadians(0), pose.getRotation().getX(), 1e-9),
        () -> assertEquals(Units.degreesToRadians(5), pose.getRotation().getY(), 1e-9),
        () -> assertEquals(Units.degreesToRadians(90), pose.getRotation().getZ(), 1e-9));
  }
}
