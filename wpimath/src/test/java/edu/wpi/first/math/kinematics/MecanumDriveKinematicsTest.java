// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation2d;
import org.junit.jupiter.api.Test;

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
        () -> assertEquals(5.0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(5.0, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(5.0, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(5.0, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testStraightLineForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(3.536, 3.536, 3.536, 3.536);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(3.536, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.omegaRadiansPerSecond, 0.1));
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
        () -> assertEquals(-4.0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(4.0, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(4.0, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(-4.0, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testStrafeForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-2.828427, 2.828427, 2.828427, -2.828427);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(2.8284, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.omegaRadiansPerSecond, 0.1));
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
        () -> assertEquals(-150.79645, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(150.79645, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-150.79645, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(150.79645, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-150.79645, 150.79645, -150.79645, 150.79645);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(2 * Math.PI, moduleStates.omegaRadiansPerSecond, 0.1));
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
        () -> assertEquals(-25.0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(29.0, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-19.0, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(23.0, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testMixedTranslationRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-17.677670, 20.51, -13.44, 16.26);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(1.413, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(2.122, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0.707, moduleStates.omegaRadiansPerSecond, 0.1));
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
        () -> assertEquals(0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(24.0, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-24.0, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(48.0, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testOffCenterRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(0, 16.971, -16.971, 33.941);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(8.48525, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(-8.48525, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0.707, moduleStates.omegaRadiansPerSecond, 0.1));
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
        () -> assertEquals(3.0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(31.0, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-17.0, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(51.0, moduleStates.rearRightMetersPerSecond, 0.1));
  }

  @Test
  void testOffCenterRotationTranslationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(2.12, 21.92, -12.02, 36.06);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(12.02, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(-7.07, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0.707, moduleStates.omegaRadiansPerSecond, 0.1));
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
    var wheelSpeeds = new MecanumDriveWheelSpeeds(5, 6, 4, 7);
    wheelSpeeds.desaturate(5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, wheelSpeeds.frontLeftMetersPerSecond, kEpsilon),
        () -> assertEquals(6.0 * factor, wheelSpeeds.frontRightMetersPerSecond, kEpsilon),
        () -> assertEquals(4.0 * factor, wheelSpeeds.rearLeftMetersPerSecond, kEpsilon),
        () -> assertEquals(7.0 * factor, wheelSpeeds.rearRightMetersPerSecond, kEpsilon));
  }
}
