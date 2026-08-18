// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;

class MecanumDriveKinematicsTest {
  private static final double kEpsilon = 1E-9;

  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final MecanumDriveKinematics m_kinematics =
      new MecanumDriveKinematics(m_fl, m_fr, m_bl, m_br);

  @Test
  void testStraightLineInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(5, 0, 0);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities);

    assertAll(
        () -> assertEquals(5.0, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(5.0, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(5.0, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(5.0, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testStraightLineForwardKinematicsKinematics() {
    var wheelVelocities = new MecanumDriveWheelVelocities(3.536, 3.536, 3.536, 3.536);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(3.536, moduleVelocities.vx, 0.1),
        () -> assertEquals(0, moduleVelocities.vy, 0.1),
        () -> assertEquals(0, moduleVelocities.omega, 0.1));
  }

  @Test
  void testStraightLineForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(3.536, 3.536, 3.536, 3.536);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(3.536, twist.dx, 0.1),
        () -> assertEquals(0, twist.dy, 0.1),
        () -> assertEquals(0, twist.dtheta, 0.1));
  }

  @Test
  void testStrafeInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(0, 4, 0);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities);

    assertAll(
        () -> assertEquals(-4.0, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(4.0, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(4.0, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(-4.0, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testStrafeForwardKinematicsKinematics() {
    var wheelVelocities = new MecanumDriveWheelVelocities(-2.828427, 2.828427, 2.828427, -2.828427);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(0, moduleVelocities.vx, 0.1),
        () -> assertEquals(2.8284, moduleVelocities.vy, 0.1),
        () -> assertEquals(0, moduleVelocities.omega, 0.1));
  }

  @Test
  void testStrafeForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(-2.828427, 2.828427, 2.828427, -2.828427);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(0, twist.dx, 0.1),
        () -> assertEquals(2.8284, twist.dy, 0.1),
        () -> assertEquals(0, twist.dtheta, 0.1));
  }

  @Test
  void testRotationInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(0, 0, 2 * Math.PI);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities);

    assertAll(
        () -> assertEquals(-150.79645, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(150.79645, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(-150.79645, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(150.79645, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testRotationForwardKinematicsKinematics() {
    var wheelVelocities =
        new MecanumDriveWheelVelocities(-150.79645, 150.79645, -150.79645, 150.79645);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(0, moduleVelocities.vx, 0.1),
        () -> assertEquals(0, moduleVelocities.vy, 0.1),
        () -> assertEquals(2 * Math.PI, moduleVelocities.omega, 0.1));
  }

  @Test
  void testRotationForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(-150.79645, 150.79645, -150.79645, 150.79645);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(0, twist.dx, 0.1),
        () -> assertEquals(0, twist.dy, 0.1),
        () -> assertEquals(2 * Math.PI, twist.dtheta, 0.1));
  }

  @Test
  void testMixedTranslationRotationInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(2, 3, 1);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities);

    assertAll(
        () -> assertEquals(-25.0, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(29.0, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(-19.0, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(23.0, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testMixedTranslationRotationForwardKinematicsKinematics() {
    var wheelVelocities = new MecanumDriveWheelVelocities(-17.677670, 20.51, -13.44, 16.26);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(1.413, moduleVelocities.vx, 0.1),
        () -> assertEquals(2.122, moduleVelocities.vy, 0.1),
        () -> assertEquals(0.707, moduleVelocities.omega, 0.1));
  }

  @Test
  void testMixedTranslationRotationForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(-17.677670, 20.51, -13.44, 16.26);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(1.413, twist.dx, 0.1),
        () -> assertEquals(2.122, twist.dy, 0.1),
        () -> assertEquals(0.707, twist.dtheta, 0.1));
  }

  @Test
  void testOffCenterRotationInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(0, 0, 1);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities, m_fl);

    assertAll(
        () -> assertEquals(0, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(24.0, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(-24.0, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(48.0, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testStraightLineInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(5, 0, 0);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(accelerations);

    assertAll(
        () -> assertEquals(5.0, wheelAccelerations.frontLeft, 0.1),
        () -> assertEquals(5.0, wheelAccelerations.frontRight, 0.1),
        () -> assertEquals(5.0, wheelAccelerations.rearLeft, 0.1),
        () -> assertEquals(5.0, wheelAccelerations.rearRight, 0.1));
  }

  @Test
  void testStraightLineForwardAccelerations() {
    var wheelAccelerations = new MecanumDriveWheelAccelerations(3.536, 3.536, 3.536, 3.536);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(3.536, chassisAccelerations.ax, 0.1),
        () -> assertEquals(0, chassisAccelerations.ay, 0.1),
        () -> assertEquals(0, chassisAccelerations.alpha, 0.1));
  }

  @Test
  void testStrafeInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(0, 4, 0);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(accelerations);

    assertAll(
        () -> assertEquals(-4.0, wheelAccelerations.frontLeft, 0.1),
        () -> assertEquals(4.0, wheelAccelerations.frontRight, 0.1),
        () -> assertEquals(4.0, wheelAccelerations.rearLeft, 0.1),
        () -> assertEquals(-4.0, wheelAccelerations.rearRight, 0.1));
  }

  @Test
  void testStrafeForwardAccelerations() {
    var wheelAccelerations =
        new MecanumDriveWheelAccelerations(-2.828427, 2.828427, 2.828427, -2.828427);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(0, chassisAccelerations.ax, 0.1),
        () -> assertEquals(2.8284, chassisAccelerations.ay, 0.1),
        () -> assertEquals(0, chassisAccelerations.alpha, 0.1));
  }

  @Test
  void testRotationInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(0, 0, 2 * Math.PI);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(accelerations);

    assertAll(
        () -> assertEquals(-150.79645, wheelAccelerations.frontLeft, 0.1),
        () -> assertEquals(150.79645, wheelAccelerations.frontRight, 0.1),
        () -> assertEquals(-150.79645, wheelAccelerations.rearLeft, 0.1),
        () -> assertEquals(150.79645, wheelAccelerations.rearRight, 0.1));
  }

  @Test
  void testRotationForwardAccelerations() {
    var wheelAccelerations =
        new MecanumDriveWheelAccelerations(-150.79645, 150.79645, -150.79645, 150.79645);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(0, chassisAccelerations.ax, 0.1),
        () -> assertEquals(0, chassisAccelerations.ay, 0.1),
        () -> assertEquals(2 * Math.PI, chassisAccelerations.alpha, 0.1));
  }

  @Test
  void testMixedTranslationRotationInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(2, 3, 1);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(accelerations);

    assertAll(
        () -> assertEquals(-25.0, wheelAccelerations.frontLeft, 0.1),
        () -> assertEquals(29.0, wheelAccelerations.frontRight, 0.1),
        () -> assertEquals(-19.0, wheelAccelerations.rearLeft, 0.1),
        () -> assertEquals(23.0, wheelAccelerations.rearRight, 0.1));
  }

  @Test
  void testMixedTranslationRotationForwardAccelerations() {
    var wheelAccelerations = new MecanumDriveWheelAccelerations(-17.677670, 20.51, -13.44, 16.26);
    var chassisAccelerations = m_kinematics.toChassisAccelerations(wheelAccelerations);

    assertAll(
        () -> assertEquals(1.413, chassisAccelerations.ax, 0.1),
        () -> assertEquals(2.122, chassisAccelerations.ay, 0.1),
        () -> assertEquals(0.707, chassisAccelerations.alpha, 0.1));
  }

  @Test
  void testOffCenterRotationInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(0, 0, 1);
    var wheelAccelerations = m_kinematics.toWheelAccelerations(accelerations, m_fl);

    assertAll(
        () -> assertEquals(0, wheelAccelerations.frontLeft, 0.1),
        () -> assertEquals(24.0, wheelAccelerations.frontRight, 0.1),
        () -> assertEquals(-24.0, wheelAccelerations.rearLeft, 0.1),
        () -> assertEquals(48.0, wheelAccelerations.rearRight, 0.1));
  }

  @Test
  void testOffCenterRotationForwardKinematicsKinematics() {
    var wheelVelocities = new MecanumDriveWheelVelocities(0, 16.971, -16.971, 33.941);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(8.48525, moduleVelocities.vx, 0.1),
        () -> assertEquals(-8.48525, moduleVelocities.vy, 0.1),
        () -> assertEquals(0.707, moduleVelocities.omega, 0.1));
  }

  @Test
  void testOffCenterRotationForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(0, 16.971, -16.971, 33.941);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(8.48525, twist.dx, 0.1),
        () -> assertEquals(-8.48525, twist.dy, 0.1),
        () -> assertEquals(0.707, twist.dtheta, 0.1));
  }

  @Test
  void testOffCenterTranslationRotationInverseKinematics() {
    ChassisVelocities velocities = new ChassisVelocities(5, 2, 1);
    var moduleVelocities = m_kinematics.toWheelVelocities(velocities, m_fl);

    assertAll(
        () -> assertEquals(3.0, moduleVelocities.frontLeft, 0.1),
        () -> assertEquals(31.0, moduleVelocities.frontRight, 0.1),
        () -> assertEquals(-17.0, moduleVelocities.rearLeft, 0.1),
        () -> assertEquals(51.0, moduleVelocities.rearRight, 0.1));
  }

  @Test
  void testOffCenterRotationTranslationForwardKinematicsKinematics() {
    var wheelVelocities = new MecanumDriveWheelVelocities(2.12, 21.92, -12.02, 36.06);
    var moduleVelocities = m_kinematics.toChassisVelocities(wheelVelocities);

    assertAll(
        () -> assertEquals(12.02, moduleVelocities.vx, 0.1),
        () -> assertEquals(-7.07, moduleVelocities.vy, 0.1),
        () -> assertEquals(0.707, moduleVelocities.omega, 0.1));
  }

  @Test
  void testOffCenterRotationTranslationForwardKinematicsKinematicsWithDeltas() {
    var wheelDeltas = new MecanumDriveWheelPositions(2.12, 21.92, -12.02, 36.06);
    var twist = m_kinematics.toTwist2d(wheelDeltas);

    assertAll(
        () -> assertEquals(12.02, twist.dx, 0.1),
        () -> assertEquals(-7.07, twist.dy, 0.1),
        () -> assertEquals(0.707, twist.dtheta, 0.1));
  }

  @Test
  void testDesaturate() {
    var wheelVelocities = new MecanumDriveWheelVelocities(5, 6, 4, 7).desaturate(5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, wheelVelocities.frontLeft, kEpsilon),
        () -> assertEquals(6.0 * factor, wheelVelocities.frontRight, kEpsilon),
        () -> assertEquals(4.0 * factor, wheelVelocities.rearLeft, kEpsilon),
        () -> assertEquals(7.0 * factor, wheelVelocities.rearRight, kEpsilon));
  }

  @Test
  void testDesaturateNegativeVelocities() {
    var wheelVelocities = new MecanumDriveWheelVelocities(-5, 6, 4, -7).desaturate(5.5);

    final double kFactor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(-5.0 * kFactor, wheelVelocities.frontLeft, kEpsilon),
        () -> assertEquals(6.0 * kFactor, wheelVelocities.frontRight, kEpsilon),
        () -> assertEquals(4.0 * kFactor, wheelVelocities.rearLeft, kEpsilon),
        () -> assertEquals(-7.0 * kFactor, wheelVelocities.rearRight, kEpsilon));
  }
}
