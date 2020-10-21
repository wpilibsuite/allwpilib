/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Translation2d;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

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

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534
      */

    assertAll(
        () -> assertEquals(3.536, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(3.536, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(3.536, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(3.536, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testStraightLineForwardKinematicsKinematics() {

    var wheelSpeeds = new MecanumDriveWheelSpeeds(3.536, 3.536, 3.536, 3.536);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534 will be [[5][0][0]]
      */

    assertAll(
        () -> assertEquals(5, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testStrafeInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 4, 0);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl -2.828427 fr 2.828427 rl 2.828427 rr -2.828427
      */

    assertAll(
        () -> assertEquals(-2.828427, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(2.828427, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(2.828427, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(-2.828427, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testStrafeForwardKinematicsKinematics() {

    var wheelSpeeds = new MecanumDriveWheelSpeeds(-2.828427, 2.828427, 2.828427, -2.828427);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534 will be [[5][0][0]]
      */

    assertAll(
        () -> assertEquals(0, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(4, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testRotationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl -106.629191 fr 106.629191 rl -106.629191 rr 106.629191
      */

    assertAll(
        () -> assertEquals(-106.629191, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(106.629191, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-106.629191, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(106.629191, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-106.629191, 106.629191, -106.629191, 106.629191);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl -106.629191 fr 106.629191 rl -106.629191 rr 106.629191 should be [[0][0][2pi]]
      */

    assertAll(
        () -> assertEquals(0, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(0, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(2 * Math.PI, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testMixedTranslationRotationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(2, 3, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds);

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl -17.677670 fr 20.506097 rl -13.435029 rr 16.263456
      */

    assertAll(
        () -> assertEquals(-17.677670, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(20.506097, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-13.435, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(16.26, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testMixedTranslationRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(-17.677670, 20.51, -13.44, 16.26);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl -17.677670 fr 20.506097 rl -13.435029 rr 16.263456 should be [[2][3][1]]
      */

    assertAll(
        () -> assertEquals(2, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(3, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(1, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testOffCenterRotationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds, m_fl);

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 0.000000 fr 16.970563 rl -16.970563 rr 33.941125
      */

    assertAll(
        () -> assertEquals(0, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(16.971, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-16.971, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(33.941, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testOffCenterRotationForwardKinematicsKinematics() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(0, 16.971, -16.971, 33.941);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from the wheel
    velocities should be [[12][-12][1]]
      */

    assertAll(
        () -> assertEquals(12, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(-12, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(1, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testOffCenterTranslationRotationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(5, 2, 1);
    var moduleStates = m_kinematics.toWheelSpeeds(speeds, m_fl);

    /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 2.121320 fr 21.920310 rl -12.020815 rr 36.062446
      */

    assertAll(
        () -> assertEquals(2.12, moduleStates.frontLeftMetersPerSecond, 0.1),
        () -> assertEquals(21.92, moduleStates.frontRightMetersPerSecond, 0.1),
        () -> assertEquals(-12.02, moduleStates.rearLeftMetersPerSecond, 0.1),
        () -> assertEquals(36.06, moduleStates.rearRightMetersPerSecond, 0.1)
    );
  }

  @Test
  void testOffCenterRotationTranslationForwardKinematicsKinematics() {

    var wheelSpeeds = new MecanumDriveWheelSpeeds(2.12, 21.92, -12.02, 36.06);
    var moduleStates = m_kinematics.toChassisSpeeds(wheelSpeeds);

    /*
    By equation (13.13) of the state-space-guide, the chassis motion from the wheel
    velocities should be [[17][-10][1]]
      */

    assertAll(
        () -> assertEquals(17, moduleStates.vxMetersPerSecond, 0.1),
        () -> assertEquals(-10, moduleStates.vyMetersPerSecond, 0.1),
        () -> assertEquals(1, moduleStates.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testNormalize() {
    var wheelSpeeds = new MecanumDriveWheelSpeeds(5, 6, 4, 7);
    wheelSpeeds.normalize(5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, wheelSpeeds.frontLeftMetersPerSecond, kEpsilon),
        () -> assertEquals(6.0 * factor, wheelSpeeds.frontRightMetersPerSecond, kEpsilon),
        () -> assertEquals(4.0 * factor, wheelSpeeds.rearLeftMetersPerSecond, kEpsilon),
        () -> assertEquals(7.0 * factor, wheelSpeeds.rearRightMetersPerSecond, kEpsilon)
    );
  }

}
