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
    ChassisSpeeds speeds = new ChassisSpeeds(5, 0, 0);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    assertAll(
        () -> assertEquals(5.0, moduleStates.frontLeft, 0.1),
        () -> assertEquals(5.0, moduleStates.frontRight, 0.1),
        () -> assertEquals(5.0, moduleStates.rearLeft, 0.1),
        () -> assertEquals(5.0, moduleStates.rearRight, 0.1));
  }

  @Test
  void testStraightLineForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(3.536, 3.536, 3.536, 3.536);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(3.536, moduleStates.vx, 0.1),
        () -> assertEquals(0, moduleStates.vy, 0.1),
        () -> assertEquals(0, moduleStates.omega, 0.1));
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
    ChassisSpeeds speeds = new ChassisSpeeds(0, 4, 0);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    assertAll(
        () -> assertEquals(-4.0, moduleStates.frontLeft, 0.1),
        () -> assertEquals(4.0, moduleStates.frontRight, 0.1),
        () -> assertEquals(4.0, moduleStates.rearLeft, 0.1),
        () -> assertEquals(-4.0, moduleStates.rearRight, 0.1));
  }

  @Test
  void testStrafeForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-2.828427, 2.828427, 2.828427, -2.828427);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0, moduleStates.vx, 0.1),
        () -> assertEquals(2.8284, moduleStates.vy, 0.1),
        () -> assertEquals(0, moduleStates.omega, 0.1));
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
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    assertAll(
        () -> assertEquals(-150.79645, moduleStates.frontLeft, 0.1),
        () -> assertEquals(150.79645, moduleStates.frontRight, 0.1),
        () -> assertEquals(-150.79645, moduleStates.rearLeft, 0.1),
        () -> assertEquals(150.79645, moduleStates.rearRight, 0.1));
  }

  @Test
  void testRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-150.79645, 150.79645, -150.79645, 150.79645);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0, moduleStates.vx, 0.1),
        () -> assertEquals(0, moduleStates.vy, 0.1),
        () -> assertEquals(2 * Math.PI, moduleStates.omega, 0.1));
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
    ChassisSpeeds speeds = new ChassisSpeeds(2, 3, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    assertAll(
        () -> assertEquals(-25.0, moduleStates.frontLeft, 0.1),
        () -> assertEquals(29.0, moduleStates.frontRight, 0.1),
        () -> assertEquals(-19.0, moduleStates.rearLeft, 0.1),
        () -> assertEquals(23.0, moduleStates.rearRight, 0.1));
  }

  @Test
  void testMixedTranslationRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-17.677670, 20.51, -13.44, 16.26);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(1.413, moduleStates.vx, 0.1),
        () -> assertEquals(2.122, moduleStates.vy, 0.1),
        () -> assertEquals(0.707, moduleStates.omega, 0.1));
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
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds, m_fl);

    assertAll(
        () -> assertEquals(0, moduleStates.frontLeft, 0.1),
        () -> assertEquals(24.0, moduleStates.frontRight, 0.1),
        () -> assertEquals(-24.0, moduleStates.rearLeft, 0.1),
        () -> assertEquals(48.0, moduleStates.rearRight, 0.1));
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
    var wheelSpeeds = new MecanumDriveWheelSpeeds(0, 16.971, -16.971, 33.941);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(8.48525, moduleStates.vx, 0.1),
        () -> assertEquals(-8.48525, moduleStates.vy, 0.1),
        () -> assertEquals(0.707, moduleStates.omega, 0.1));
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
    ChassisSpeeds speeds = new ChassisSpeeds(5, 2, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds, m_fl);

    assertAll(
        () -> assertEquals(3.0, moduleStates.frontLeft, 0.1),
        () -> assertEquals(31.0, moduleStates.frontRight, 0.1),
        () -> assertEquals(-17.0, moduleStates.rearLeft, 0.1),
        () -> assertEquals(51.0, moduleStates.rearRight, 0.1));
  }

  @Test
  void testOffCenterRotationTranslationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(2.12, 21.92, -12.02, 36.06);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(12.02, moduleStates.vx, 0.1),
        () -> assertEquals(-7.07, moduleStates.vy, 0.1),
        () -> assertEquals(0.707, moduleStates.omega, 0.1));
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
    var wheelSpeeds = new MecanumDriveWheelSpeeds(5, 6, 4, 7).desaturate(5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, wheelSpeeds.frontLeft, kEpsilon),
        () -> assertEquals(6.0 * factor, wheelSpeeds.frontRight, kEpsilon),
        () -> assertEquals(4.0 * factor, wheelSpeeds.rearLeft, kEpsilon),
        () -> assertEquals(7.0 * factor, wheelSpeeds.rearRight, kEpsilon));
  }

  @Test
  void testDesaturateNegativeSpeeds() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-5, 6, 4, -7).desaturate(5.5);

    final double kFactor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(-5.0 * kFactor, wheelSpeeds.frontLeft, kEpsilon),
        () -> assertEquals(6.0 * kFactor, wheelSpeeds.frontRight, kEpsilon),
        () -> assertEquals(4.0 * kFactor, wheelSpeeds.rearLeft, kEpsilon),
        () -> assertEquals(-7.0 * kFactor, wheelSpeeds.rearRight, kEpsilon));
  }
}
