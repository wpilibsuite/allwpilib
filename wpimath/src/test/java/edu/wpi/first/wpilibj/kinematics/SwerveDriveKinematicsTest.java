/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.kinematics;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class SwerveDriveKinematicsTest {
  private static final double kEpsilon = 1E-9;

  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final SwerveDriveKinematics m_kinematics =
      new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br);

  @Test
  void testStraightLineInverseKinematics() { // test inverse kinematics going in a straight line

    ChassisSpeeds speeds = new ChassisSpeeds(5, 0, 0);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds);

    assertAll(
        () -> assertEquals(5.0, moduleStates[0].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[1].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[2].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[3].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, moduleStates[0].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[2].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[3].angle.getRadians(), kEpsilon)
    );
  }

  @Test
  void testStraightLineForwardKinematics() { // test forward kinematics going in a straight line
    SwerveModuleState state = new SwerveModuleState(5.0, Rotation2d.fromDegrees(90.0));
    var chassisSpeeds = m_kinematics.toChassisSpeeds(state, state, state, state);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omegaRadiansPerSecond, kEpsilon)
    );
  }

  @Test
  void testStraightStrafeInverseKinematics() {

    ChassisSpeeds speeds = new ChassisSpeeds(0, 5, 0);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds);

    assertAll(
        () -> assertEquals(5.0, moduleStates[0].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[1].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[2].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, moduleStates[3].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(90.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[3].angle.getDegrees(), kEpsilon)
    );
  }

  @Test
  void testStraightStrafeForwardKinematics() {
    SwerveModuleState state = new SwerveModuleState(5.0, Rotation2d.fromDegrees(90.0));
    var chassisSpeeds = m_kinematics.toChassisSpeeds(state, state, state, state);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(5.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omegaRadiansPerSecond, kEpsilon)
    );
  }

  @Test
  void testTurnInPlaceInverseKinematics() {

    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds);

    /*
    The circumference of the wheels about the COR is pi * diameter, or 2 * pi * radius
    the radius is the sqrt(12^2in + 12^2in), or 16.9706in, so the circumference the wheels
    trace out is 106.629190516in. since we want our robot to rotate at 1 rotation per second,
    our wheels must trace out 1 rotation (or 106.63 inches) per second.
      */

    assertAll(
        () -> assertEquals(106.63, moduleStates[0].speedMetersPerSecond, 0.1),
        () -> assertEquals(106.63, moduleStates[1].speedMetersPerSecond, 0.1),
        () -> assertEquals(106.63, moduleStates[2].speedMetersPerSecond, 0.1),
        () -> assertEquals(106.63, moduleStates[3].speedMetersPerSecond, 0.1),
        () -> assertEquals(135.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(45.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-135.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-45.0, moduleStates[3].angle.getDegrees(), kEpsilon)
    );
  }

  @Test
  void testTurnInPlaceForwardKinematics() {
    SwerveModuleState flState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(135));
    SwerveModuleState frState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(45));
    SwerveModuleState blState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(-135));
    SwerveModuleState brState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(-45));

    var chassisSpeeds = m_kinematics.toChassisSpeeds(flState, frState, blState, brState);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vyMetersPerSecond, kEpsilon),
        () -> assertEquals(2 * Math.PI, chassisSpeeds.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testOffCenterCORRotationInverseKinematics() {

    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds, m_fl);

    /*
    This one is a bit trickier. Because we are rotating about the front-left wheel,
    it should be parked at 0 degrees and 0 speed. The front-right and back-left wheels both travel
    an arc with radius 24 (and circumference 150.796), and the back-right wheel travels an arc with
    radius sqrt(24^2 + 24^2) and circumference 213.2584. As for angles, the front-right wheel
    should be pointing straight forward, the back-left wheel should be pointing straight right,
    and the back-right wheel should be at a -45 degree angle
    */

    assertAll(
        () -> assertEquals(0.0, moduleStates[0].speedMetersPerSecond, 0.1),
        () -> assertEquals(150.796, moduleStates[1].speedMetersPerSecond, 0.1),
        () -> assertEquals(150.796, moduleStates[2].speedMetersPerSecond, 0.1),
        () -> assertEquals(213.258, moduleStates[3].speedMetersPerSecond, 0.1),
        () -> assertEquals(0.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-90.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-45.0, moduleStates[3].angle.getDegrees(), kEpsilon)
    );
  }

  @Test
  void testOffCenterCORRotationForwardKinematics() {
    SwerveModuleState flState = new SwerveModuleState(0.0, Rotation2d.fromDegrees(0.0));
    SwerveModuleState frState = new SwerveModuleState(150.796, Rotation2d.fromDegrees(0.0));
    SwerveModuleState blState = new SwerveModuleState(150.796, Rotation2d.fromDegrees(-90));
    SwerveModuleState brState = new SwerveModuleState(213.258, Rotation2d.fromDegrees(-45));

    var chassisSpeeds = m_kinematics.toChassisSpeeds(flState, frState, blState, brState);

    /*
    We already know that our omega should be 2pi from the previous test. Next, we need to determine
    the vx and vy of our chassis center. Because our COR is at a 45 degree angle from the center,
    we know that vx and vy must be the same. Furthermore, we know that the center of mass makes
    a full revolution about the center of revolution once every second. Therefore, the center of
    mass must be moving at 106.629in/sec. Recalling that the ratios of a 45/45/90 triagle are
    1:sqrt(2)/2:sqrt(2)/2, we find that the COM vx is -75.398, and vy is 75.398.
    */

    assertAll(
        () -> assertEquals(75.398, chassisSpeeds.vxMetersPerSecond, 0.1),
        () -> assertEquals(-75.398, chassisSpeeds.vyMetersPerSecond, 0.1),
        () -> assertEquals(2 * Math.PI, chassisSpeeds.omegaRadiansPerSecond, 0.1)
    );
  }

  private void assertModuleState(SwerveModuleState expected, SwerveModuleState actual,
                                 SwerveModuleState tolerance) {
    assertAll(
        () -> assertEquals(expected.speedMetersPerSecond, actual.speedMetersPerSecond,
            tolerance.speedMetersPerSecond),
        () -> assertEquals(expected.angle.getDegrees(), actual.angle.getDegrees(),
            tolerance.angle.getDegrees())
    );
  }

  /**
   * Test the rotation of the robot about a non-central point with
   * both linear and angular velocities.
   */
  @Test
  void testOffCenterCORRotationAndTranslationInverseKinematics() {

    ChassisSpeeds speeds = new ChassisSpeeds(0.0, 3.0, 1.5);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds, new Translation2d(24, 0));

    // By equation (13.14) from state-space guide, our wheels/angles will be as follows,
    // (+-1 degree or speed):
    SwerveModuleState[] expectedStates = new SwerveModuleState[]{
        new SwerveModuleState(23.43, Rotation2d.fromDegrees(-140.19)),
        new SwerveModuleState(23.43, Rotation2d.fromDegrees(-39.81)),
        new SwerveModuleState(54.08, Rotation2d.fromDegrees(-109.44)),
        new SwerveModuleState(54.08, Rotation2d.fromDegrees(-70.56))
    };
    var stateTolerance = new SwerveModuleState(0.1, Rotation2d.fromDegrees(0.1));

    for (int i = 0; i < expectedStates.length; i++) {
      assertModuleState(expectedStates[i], moduleStates[i], stateTolerance);
    }
  }

  @Test
  void testOffCenterCORRotationAndTranslationForwardKinematics() {
    SwerveModuleState flState = new SwerveModuleState(23.43, Rotation2d.fromDegrees(-140.19));
    SwerveModuleState frState = new SwerveModuleState(23.43, Rotation2d.fromDegrees(-39.81));
    SwerveModuleState blState = new SwerveModuleState(54.08, Rotation2d.fromDegrees(-109.44));
    SwerveModuleState brState = new SwerveModuleState(54.08, Rotation2d.fromDegrees(-70.56));

    var chassisSpeeds = m_kinematics.toChassisSpeeds(flState, frState, blState, brState);

    /*
    From equation (13.17), we know that chassis motion is th dot product of the
    pseudoinverse of the inverseKinematics matrix (with the center of rotation at
    (0,0) -- we don't want the motion of the center of rotation, we want it of
    the center of the robot). These above SwerveModuleStates are known to be from
    a velocity of [[0][3][1.5]] about (0, 24), and the expected numbers have been
    calculated using Numpy's linalg.pinv function.
    */

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vxMetersPerSecond, 0.1),
        () -> assertEquals(-33.0, chassisSpeeds.vyMetersPerSecond, 0.1),
        () -> assertEquals(1.5, chassisSpeeds.omegaRadiansPerSecond, 0.1)
    );
  }

  @Test
  void testNormalize() {
    SwerveModuleState fl = new SwerveModuleState(5, new Rotation2d());
    SwerveModuleState fr = new SwerveModuleState(6, new Rotation2d());
    SwerveModuleState bl = new SwerveModuleState(4, new Rotation2d());
    SwerveModuleState br = new SwerveModuleState(7, new Rotation2d());

    SwerveModuleState[] arr = {fl, fr, bl, br};
    SwerveDriveKinematics.normalizeWheelSpeeds(arr, 5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, arr[0].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(6.0 * factor, arr[1].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(4.0 * factor, arr[2].speedMetersPerSecond, kEpsilon),
        () -> assertEquals(7.0 * factor, arr[3].speedMetersPerSecond, kEpsilon)
    );
  }

}
