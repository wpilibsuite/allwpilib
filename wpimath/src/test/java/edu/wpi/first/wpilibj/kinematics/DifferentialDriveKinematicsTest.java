/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class DifferentialDriveKinematicsTest {
  private static final double kEpsilon = 1E-9;
  private final DifferentialDriveKinematics m_kinematics
      = new DifferentialDriveKinematics(0.381 * 2);

  @Test
  void testInverseKinematicsForZeros() {
    var chassisSpeeds = new ChassisSpeeds();
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(0.0, wheelSpeeds.leftMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, wheelSpeeds.rightMetersPerSecond, kEpsilon)
    );
  }

  @Test
  void testForwardKinematicsForZeros() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds();
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omegaRadiansPerSecond, kEpsilon)
    );
  }

  @Test
  void testInverseKinematicsForStraightLine() {
    var chassisSpeeds = new ChassisSpeeds(3, 0, 0);
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(3.0, wheelSpeeds.leftMetersPerSecond, kEpsilon),
        () -> assertEquals(3.0, wheelSpeeds.rightMetersPerSecond, kEpsilon)
    );
  }

  @Test
  void testForwardKinematicsForStraightLine() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds(3, 3);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(3.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omegaRadiansPerSecond, kEpsilon)
    );
  }

  @Test
  void testInverseKinematicsForRotateInPlace() {
    var chassisSpeeds = new ChassisSpeeds(0, 0, Math.PI);
    var wheelSpeeds = m_kinematics.toWheelSpeeds(chassisSpeeds);

    assertAll(
        () -> assertEquals(-0.381 * Math.PI, wheelSpeeds.leftMetersPerSecond, kEpsilon),
        () -> assertEquals(+0.381 * Math.PI, wheelSpeeds.rightMetersPerSecond, kEpsilon)
    );
  }

  @Test
  void testForwardKinematicsForRotateInPlace() {
    var wheelSpeeds = new DifferentialDriveWheelSpeeds(+0.381 * Math.PI, -0.381 * Math.PI);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(wheelSpeeds);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(-Math.PI, chassisSpeeds.omegaRadiansPerSecond, kEpsilon)
    );
  }
}
