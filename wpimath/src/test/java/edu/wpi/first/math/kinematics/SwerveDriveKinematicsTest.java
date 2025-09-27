// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import org.junit.jupiter.api.Test;

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
        () -> assertEquals(5.0, moduleStates[0].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[1].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[2].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[3].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[0].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[2].angle.getRadians(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[3].angle.getRadians(), kEpsilon));
  }

  @Test
  void testStraightLineForwardKinematics() { // test forward kinematics going in a straight line
    SwerveModuleState state = new SwerveModuleState(5.0, Rotation2d.kZero);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(state, state, state, state);

    assertAll(
        () -> assertEquals(5.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testStraightLineForwardKinematicsWithDeltas() {
    // test forward kinematics going in a straight line
    SwerveModulePosition delta = new SwerveModulePosition(5.0, Rotation2d.kZero);
    var twist = m_kinematics.toTwist2d(delta, delta, delta, delta);

    assertAll(
        () -> assertEquals(5.0, twist.dx, kEpsilon),
        () -> assertEquals(0.0, twist.dy, kEpsilon),
        () -> assertEquals(0.0, twist.dtheta, kEpsilon));
  }

  @Test
  void testStraightStrafeInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 5, 0);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds);

    assertAll(
        () -> assertEquals(5.0, moduleStates[0].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[1].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[2].speed, kEpsilon),
        () -> assertEquals(5.0, moduleStates[3].speed, kEpsilon),
        () -> assertEquals(90.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[3].angle.getDegrees(), kEpsilon));
  }

  @Test
  void testStraightStrafeForwardKinematics() {
    SwerveModuleState state = new SwerveModuleState(5.0, Rotation2d.kCCW_Pi_2);
    var chassisSpeeds = m_kinematics.toChassisSpeeds(state, state, state, state);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(5.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.omega, kEpsilon));
  }

  @Test
  void testStraightStrafeForwardKinematicsWithDeltas() {
    SwerveModulePosition delta = new SwerveModulePosition(5.0, Rotation2d.kCCW_Pi_2);
    var twist = m_kinematics.toTwist2d(delta, delta, delta, delta);

    assertAll(
        () -> assertEquals(0.0, twist.dx, kEpsilon),
        () -> assertEquals(5.0, twist.dy, kEpsilon),
        () -> assertEquals(0.0, twist.dtheta, kEpsilon));
  }

  @Test
  void testConserveWheelAngle() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    m_kinematics.toSwerveModuleStates(speeds);
    var moduleStates = m_kinematics.toSwerveModuleStates(new ChassisSpeeds());

    // Robot is stationary, but module angles are preserved.

    assertAll(
        () -> assertEquals(0.0, moduleStates[0].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[2].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[3].speed, kEpsilon),
        () -> assertEquals(135.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(45.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-135.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-45.0, moduleStates[3].angle.getDegrees(), kEpsilon));
  }

  @Test
  void testResetWheelAngle() {
    Rotation2d fl = Rotation2d.kZero;
    Rotation2d fr = Rotation2d.kCCW_Pi_2;
    Rotation2d bl = Rotation2d.kPi;
    Rotation2d br = new Rotation2d(3 * Math.PI / 2);
    m_kinematics.resetHeadings(fl, fr, bl, br);
    var moduleStates = m_kinematics.toSwerveModuleStates(new ChassisSpeeds());

    // Robot is stationary, but module angles are preserved.

    assertAll(
        () -> assertEquals(0.0, moduleStates[0].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[2].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[3].speed, kEpsilon),
        () -> assertEquals(0.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(90.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(180.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-90.0, moduleStates[3].angle.getDegrees(), kEpsilon));
  }

  @Test
  void testTurnInPlaceInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds);

    /*
    The circumference of the wheels about the COR is π * diameter, or 2π * radius
    the radius is the √(12²in + 12²in), or 16.9706in, so the circumference the wheels
    trace out is 106.629190516in. since we want our robot to rotate at 1 rotation per second,
    our wheels must trace out 1 rotation (or 106.63 inches) per second.
      */

    assertAll(
        () -> assertEquals(106.63, moduleStates[0].speed, 0.1),
        () -> assertEquals(106.63, moduleStates[1].speed, 0.1),
        () -> assertEquals(106.63, moduleStates[2].speed, 0.1),
        () -> assertEquals(106.63, moduleStates[3].speed, 0.1),
        () -> assertEquals(135.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(45.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-135.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-45.0, moduleStates[3].angle.getDegrees(), kEpsilon));
  }

  @Test
  void testTurnInPlaceForwardKinematics() {
    SwerveModuleState flState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(135));
    SwerveModuleState frState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(45));
    SwerveModuleState blState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(-135));
    SwerveModuleState brState = new SwerveModuleState(106.629, Rotation2d.fromDegrees(-45));

    var chassisSpeeds = m_kinematics.toChassisSpeeds(flState, frState, blState, brState);

    assertAll(
        () -> assertEquals(0.0, chassisSpeeds.vx, kEpsilon),
        () -> assertEquals(0.0, chassisSpeeds.vy, kEpsilon),
        () -> assertEquals(2 * Math.PI, chassisSpeeds.omega, 0.1));
  }

  @Test
  void testTurnInPlaceForwardKinematicsWithDeltas() {
    SwerveModulePosition flDelta = new SwerveModulePosition(106.629, Rotation2d.fromDegrees(135));
    SwerveModulePosition frDelta = new SwerveModulePosition(106.629, Rotation2d.fromDegrees(45));
    SwerveModulePosition blDelta = new SwerveModulePosition(106.629, Rotation2d.fromDegrees(-135));
    SwerveModulePosition brDelta = new SwerveModulePosition(106.629, Rotation2d.fromDegrees(-45));

    var twist = m_kinematics.toTwist2d(flDelta, frDelta, blDelta, brDelta);

    assertAll(
        () -> assertEquals(0.0, twist.dx, kEpsilon),
        () -> assertEquals(0.0, twist.dy, kEpsilon),
        () -> assertEquals(2 * Math.PI, twist.dtheta, 0.1));
  }

  @Test
  void testOffCenterCORRotationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0, 0, 2 * Math.PI);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds, m_fl);

    /*
    This one is a bit trickier. Because we are rotating about the front-left wheel,
    it should be parked at 0 degrees and 0 speed. The front-right and back-left wheels both travel
    an arc with radius 24 (and circumference 150.796), and the back-right wheel travels an arc with
    radius √(24² + 24²) and circumference 213.2584. As for angles, the front-right wheel
    should be pointing straight forward, the back-left wheel should be pointing straight right,
    and the back-right wheel should be at a -45 degree angle
    */

    assertAll(
        () -> assertEquals(0.0, moduleStates[0].speed, 0.1),
        () -> assertEquals(150.796, moduleStates[1].speed, 0.1),
        () -> assertEquals(150.796, moduleStates[2].speed, 0.1),
        () -> assertEquals(213.258, moduleStates[3].speed, 0.1),
        () -> assertEquals(0.0, moduleStates[0].angle.getDegrees(), kEpsilon),
        () -> assertEquals(0.0, moduleStates[1].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-90.0, moduleStates[2].angle.getDegrees(), kEpsilon),
        () -> assertEquals(-45.0, moduleStates[3].angle.getDegrees(), kEpsilon));
  }

  @Test
  void testOffCenterCORRotationForwardKinematics() {
    SwerveModuleState flState = new SwerveModuleState(0.0, Rotation2d.kZero);
    SwerveModuleState frState = new SwerveModuleState(150.796, Rotation2d.kZero);
    SwerveModuleState blState = new SwerveModuleState(150.796, Rotation2d.kCW_Pi_2);
    SwerveModuleState brState = new SwerveModuleState(213.258, Rotation2d.fromDegrees(-45));

    var chassisSpeeds = m_kinematics.toChassisSpeeds(flState, frState, blState, brState);

    /*
    We already know that our omega should be 2π from the previous test. Next, we need to determine
    the vx and vy of our chassis center. Because our COR is at a 45 degree angle from the center,
    we know that vx and vy must be the same. Furthermore, we know that the center of mass makes
    a full revolution about the center of revolution once every second. Therefore, the center of
    mass must be moving at 106.629in/sec. Recalling that the ratios of a 45/45/90 triangle are
    1:√(2)/2:√(2)/2, we find that the COM vx is -75.398, and vy is 75.398.
    */

    assertAll(
        () -> assertEquals(75.398, chassisSpeeds.vx, 0.1),
        () -> assertEquals(-75.398, chassisSpeeds.vy, 0.1),
        () -> assertEquals(2 * Math.PI, chassisSpeeds.omega, 0.1));
  }

  @Test
  void testOffCenterCORRotationForwardKinematicsWithDeltas() {
    SwerveModulePosition flDelta = new SwerveModulePosition(0.0, Rotation2d.kZero);
    SwerveModulePosition frDelta = new SwerveModulePosition(150.796, Rotation2d.kZero);
    SwerveModulePosition blDelta = new SwerveModulePosition(150.796, Rotation2d.fromDegrees(-90));
    SwerveModulePosition brDelta = new SwerveModulePosition(213.258, Rotation2d.fromDegrees(-45));

    var twist = m_kinematics.toTwist2d(flDelta, frDelta, blDelta, brDelta);

    /*
    We already know that our omega should be 2π from the previous test. Next, we need to determine
    the vx and vy of our chassis center. Because our COR is at a 45 degree angle from the center,
    we know that vx and vy must be the same. Furthermore, we know that the center of mass makes
    a full revolution about the center of revolution once every second. Therefore, the center of
    mass must be moving at 106.629in/sec. Recalling that the ratios of a 45/45/90 triangle are
    1:√(2)/2:√(2)/2, we find that the COM vx is -75.398, and vy is 75.398.
    */

    assertAll(
        () -> assertEquals(75.398, twist.dx, 0.1),
        () -> assertEquals(-75.398, twist.dy, 0.1),
        () -> assertEquals(2 * Math.PI, twist.dtheta, 0.1));
  }

  private void assertModuleState(
      SwerveModuleState expected, SwerveModuleState actual, SwerveModuleState tolerance) {
    assertAll(
        () -> assertEquals(expected.speed, actual.speed, tolerance.speed),
        () ->
            assertEquals(
                expected.angle.getDegrees(),
                actual.angle.getDegrees(),
                tolerance.angle.getDegrees()));
  }

  /**
   * Test the rotation of the robot about a non-central point with both linear and angular
   * velocities.
   */
  @Test
  void testOffCenterCORRotationAndTranslationInverseKinematics() {
    ChassisSpeeds speeds = new ChassisSpeeds(0.0, 3.0, 1.5);
    var moduleStates = m_kinematics.toSwerveModuleStates(speeds, new Translation2d(24, 0));

    // By equation (13.14) from state-space guide, our wheels/angles will be as follows,
    // (+-1 degree or speed):
    SwerveModuleState[] expectedStates =
        new SwerveModuleState[] {
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
        () -> assertEquals(0.0, chassisSpeeds.vx, 0.1),
        () -> assertEquals(-33.0, chassisSpeeds.vy, 0.1),
        () -> assertEquals(1.5, chassisSpeeds.omega, 0.1));
  }

  @Test
  void testOffCenterCORRotationAndTranslationForwardKinematicsWithDeltas() {
    SwerveModulePosition flDelta = new SwerveModulePosition(23.43, Rotation2d.fromDegrees(-140.19));
    SwerveModulePosition frDelta = new SwerveModulePosition(23.43, Rotation2d.fromDegrees(-39.81));
    SwerveModulePosition blDelta = new SwerveModulePosition(54.08, Rotation2d.fromDegrees(-109.44));
    SwerveModulePosition brDelta = new SwerveModulePosition(54.08, Rotation2d.fromDegrees(-70.56));

    var twist = m_kinematics.toTwist2d(flDelta, frDelta, blDelta, brDelta);

    /*
    From equation (13.17), we know that chassis motion is th dot product of the
    pseudoinverse of the inverseKinematics matrix (with the center of rotation at
    (0,0) -- we don't want the motion of the center of rotation, we want it of
    the center of the robot). These above SwerveModuleStates are known to be from
    a velocity of [[0][3][1.5]] about (0, 24), and the expected numbers have been
    calculated using Numpy's linalg.pinv function.
    */

    assertAll(
        () -> assertEquals(0.0, twist.dx, 0.1),
        () -> assertEquals(-33.0, twist.dy, 0.1),
        () -> assertEquals(1.5, twist.dtheta, 0.1));
  }

  @Test
  void testDesaturate() {
    SwerveModuleState fl = new SwerveModuleState(5, Rotation2d.kZero);
    SwerveModuleState fr = new SwerveModuleState(6, Rotation2d.kZero);
    SwerveModuleState bl = new SwerveModuleState(4, Rotation2d.kZero);
    SwerveModuleState br = new SwerveModuleState(7, Rotation2d.kZero);

    SwerveModuleState[] arr = {fl, fr, bl, br};
    SwerveDriveKinematics.desaturateWheelSpeeds(arr, 5.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, arr[0].speed, kEpsilon),
        () -> assertEquals(6.0 * factor, arr[1].speed, kEpsilon),
        () -> assertEquals(4.0 * factor, arr[2].speed, kEpsilon),
        () -> assertEquals(7.0 * factor, arr[3].speed, kEpsilon));
  }

  @Test
  void testDesaturateSmooth() {
    SwerveModuleState fl = new SwerveModuleState(5, Rotation2d.kZero);
    SwerveModuleState fr = new SwerveModuleState(6, Rotation2d.kZero);
    SwerveModuleState bl = new SwerveModuleState(4, Rotation2d.kZero);
    SwerveModuleState br = new SwerveModuleState(7, Rotation2d.kZero);

    SwerveModuleState[] arr = {fl, fr, bl, br};
    SwerveDriveKinematics.desaturateWheelSpeeds(
        arr, m_kinematics.toChassisSpeeds(arr), 5.5, 5.5, 3.5);

    double factor = 5.5 / 7.0;

    assertAll(
        () -> assertEquals(5.0 * factor, arr[0].speed, kEpsilon),
        () -> assertEquals(6.0 * factor, arr[1].speed, kEpsilon),
        () -> assertEquals(4.0 * factor, arr[2].speed, kEpsilon),
        () -> assertEquals(7.0 * factor, arr[3].speed, kEpsilon));
  }

  @Test
  void testDesaturateNegativeSpeed() {
    SwerveModuleState fl = new SwerveModuleState(1, Rotation2d.kZero);
    SwerveModuleState fr = new SwerveModuleState(1, Rotation2d.kZero);
    SwerveModuleState bl = new SwerveModuleState(-2, Rotation2d.kZero);
    SwerveModuleState br = new SwerveModuleState(-2, Rotation2d.kZero);

    SwerveModuleState[] arr = {fl, fr, bl, br};
    SwerveDriveKinematics.desaturateWheelSpeeds(arr, 1);

    assertAll(
        () -> assertEquals(0.5, arr[0].speed, kEpsilon),
        () -> assertEquals(0.5, arr[1].speed, kEpsilon),
        () -> assertEquals(-1.0, arr[2].speed, kEpsilon),
        () -> assertEquals(-1.0, arr[3].speed, kEpsilon));
  }

  @Test
  void testTurnInPlaceInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(0, 0, 2 * Math.PI);
    double angularVelocity = 2 * Math.PI;
    var moduleAccelerations =
        m_kinematics.toSwerveModuleAccelerations(accelerations, angularVelocity);

    /*
    For turn-in-place with angular acceleration of 2π rad/s² and angular velocity of 2π rad/s,
    each module has both tangential acceleration (from angular acceleration) and centripetal
    acceleration (from angular velocity). The total acceleration magnitude is approximately 678.4.

    The acceleration angles are not simply tangential (as in pure rotation) because the
    centripetal component from the existing angular velocity affects the direction:
    FL: -144°, FR: 126°, BL: -54°, BR: 36°

    These angles reflect the combination of:
    - Tangential acceleration from angular acceleration (2π rad/s²)
    - Centripetal acceleration from angular velocity (2π rad/s)
    */

    assertAll(
        () -> assertEquals(678.4, moduleAccelerations[0].acceleration, 0.1),
        () -> assertEquals(678.4, moduleAccelerations[1].acceleration, 0.1),
        () -> assertEquals(678.4, moduleAccelerations[2].acceleration, 0.1),
        () -> assertEquals(678.4, moduleAccelerations[3].acceleration, 0.1),
        () -> assertEquals(-144.0, moduleAccelerations[0].angle.getDegrees(), 0.1),
        () -> assertEquals(126.0, moduleAccelerations[1].angle.getDegrees(), 0.1),
        () -> assertEquals(-54.0, moduleAccelerations[2].angle.getDegrees(), 0.1),
        () -> assertEquals(36.0, moduleAccelerations[3].angle.getDegrees(), 0.1));
  }

  @Test
  void testTurnInPlaceForwardAccelerations() {
    SwerveModuleAccelerations flAccel =
        new SwerveModuleAccelerations(106.629, Rotation2d.fromDegrees(135));
    SwerveModuleAccelerations frAccel =
        new SwerveModuleAccelerations(106.629, Rotation2d.fromDegrees(45));
    SwerveModuleAccelerations blAccel =
        new SwerveModuleAccelerations(106.629, Rotation2d.fromDegrees(-135));
    SwerveModuleAccelerations brAccel =
        new SwerveModuleAccelerations(106.629, Rotation2d.fromDegrees(-45));

    var chassisAccelerations =
        m_kinematics.toChassisAccelerations(flAccel, frAccel, blAccel, brAccel);

    assertAll(
        () -> assertEquals(0.0, chassisAccelerations.ax, 0.1),
        () -> assertEquals(0.0, chassisAccelerations.ay, 0.1),
        () -> assertEquals(2 * Math.PI, chassisAccelerations.alpha, 0.1));
  }

  @Test
  void testOffCenterRotationInverseAccelerations() {
    ChassisAccelerations accelerations = new ChassisAccelerations(0, 0, 1);
    // For this test, assume an angular velocity of 1 rad/s
    double angularVelocity = 1.0;
    var moduleAccelerations =
        m_kinematics.toSwerveModuleAccelerations(accelerations, angularVelocity, m_fl);

    /*
    When rotating about the front-left module position with both angular acceleration (1 rad/s²)
    and angular velocity (1 rad/s), each module experiences both tangential and centripetal
    accelerations that combine vectorially.

    FL: at center of rotation (0 acceleration)
    FR: 24 units from center, experiences combined acceleration → ~33.94
    BL: 24 units from center, experiences combined acceleration → ~33.94
    BR: ~33.94 units from center, experiences combined acceleration → ~48.0

    Angles reflect the vector combination of tangential and centripetal components:
    FL: -45° (though magnitude is 0), FR: 45°, BL: -45°, BR: 0°
    */

    assertAll(
        () -> assertEquals(0, moduleAccelerations[0].acceleration, 0.1),
        () -> assertEquals(33.94, moduleAccelerations[1].acceleration, 0.1),
        () -> assertEquals(33.94, moduleAccelerations[2].acceleration, 0.1),
        () -> assertEquals(48.0, moduleAccelerations[3].acceleration, 0.1),
        () -> assertEquals(0.0, moduleAccelerations[0].angle.getDegrees(), 0.1),
        () -> assertEquals(45.0, moduleAccelerations[1].angle.getDegrees(), 0.1),
        () -> assertEquals(-45.0, moduleAccelerations[2].angle.getDegrees(), 0.1),
        () -> assertEquals(0.0, moduleAccelerations[3].angle.getDegrees(), 0.1));
  }
}
