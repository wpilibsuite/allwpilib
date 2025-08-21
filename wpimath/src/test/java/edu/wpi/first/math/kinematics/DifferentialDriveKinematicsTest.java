// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class DifferentialDriveKinematicsTest {
  private static final double kEpsilon = 1E-9;
  private final DifferentialDriveKinematics m_kinematics =
      new DifferentialDriveKinematics(0.381 * 2);

  @Test
  void testInverseKinematicsForZeros() {
    var chassisSpeeds = new ChassisSpeeds();
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(0.0, wheelSpeeds.left, kEpsilon),
        () -> assertEquals(0.0, wheelSpeeds.right, kEpsilon));
  }

  @Test
  void testForwardKinematicsForZeros() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds();
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testInverseKinematicsForStraightLine() {
    var chassisSpeeds = new ChassisSpeeds(3, 0, 0);
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(3.0, wheelSpeeds.left, kEpsilon),
        () -> assertEquals(3.0, wheelSpeeds.right, kEpsilon));
  }

  @Test
  void testForwardKinematicsForStraightLine() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds(3, 3);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(3.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testInverseKinematicsForRotateInPlace() {
    var chassisSpeeds = new ChassisSpeeds(0, 0, Math.PI);
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(-0.381 * Math.PI, wheelSpeeds.left, kEpsilon),
        () -> assertEquals(+0.381 * Math.PI, wheelSpeeds.right, kEpsilon));
  }

  @Test
  void testForwardKinematicsForRotateInPlace() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds(+0.381 * Math.PI, -0.381 * Math.PI);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(-Math.PI, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testInverseAccelerationsForZeros() {
    var chassisAccelerations = new ChassisAccelerations();
    var wheelAccelerations = m_kinematics.toWheelAccelerations(chassisAccelerations);

    assertAll(
        () -> assertEquals(0.0, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(0.0, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testForwardAccelerationsForZeros() {
    var wheelAccelerations = new DifferentialDriveWheelAccelerations();
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(0.0, chassisAccelerations.ax, kEpsilon),
        () -> assertEquals(0.0, chassisAccelerations.ay, kEpsilon),
        () -> assertEquals(0.0, chassisAccelerations.alpha, kEpsilon));
  }

  @Test
  void testInverseAccelerationsForStraightLine() {
    var chassisAccelerations = new ChassisAccelerations(3, 0, 0);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(chassisAccelerations);

    assertAll(
        () -> assertEquals(3.0, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(3.0, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testForwardAccelerationsForStraightLine() {
    var wheelAccelerations = new DifferentialDriveWheelAccelerations(3, 3);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(3.0, chassisAccelerations.ax, kEpsilon),
        () -> assertEquals(0.0, chassisAccelerations.ay, kEpsilon),
        () -> assertEquals(0.0, chassisAccelerations.alpha, kEpsilon));
  }

  @Test
  void testInverseAccelerationsForRotateInPlace() {
    var chassisAccelerations = new ChassisAccelerations(0, 0, Math.PI);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(chassisAccelerations);

    assertAll(
        () -> assertEquals(-0.381 * Math.PI, wheelAccelerations.left, kEpsilon),
        () -> assertEquals(+0.381 * Math.PI, wheelAccelerations.right, kEpsilon));
  }

  @Test
  void testForwardAccelerationsForRotateInPlace() {
    var wheelAccelerations = new DifferentialDriveWheelAccelerations(+0.381 * Math.PI, -0.381 * Math.PI);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(0.0, chassisAccelerations.ax, kEpsilon),
        () -> assertEquals(0.0, chassisAccelerations.ay, kEpsilon),
        () -> assertEquals(-Math.PI, chassisAccelerations.alpha, kEpsilon));
  }
}
