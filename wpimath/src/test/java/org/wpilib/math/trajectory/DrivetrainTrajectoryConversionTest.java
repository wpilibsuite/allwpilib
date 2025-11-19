// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.util.Units.feetToMeters;
import static org.wpilib.units.Units.Seconds;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveWheelSpeeds;
import org.wpilib.math.kinematics.MecanumDriveKinematics;
import org.wpilib.math.kinematics.MecanumDriveWheelSpeeds;
import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.math.kinematics.SwerveModuleState;

class DrivetrainTrajectoryConversionTest {
  private static final double kEpsilon = 1e-6;

  /**
   * Generates a sample trajectory similar to the 2018 cross scale auto.
   *
   * @return A spline trajectory for testing.
   */
  static Trajectory<SplineSample> generateTestTrajectory() {
    final double maxVelocity = feetToMeters(12.0);
    final double maxAccel = feetToMeters(12);

    // 2018 cross scale auto waypoints.
    var sideStart = new Pose2d(feetToMeters(1.54), feetToMeters(23.23), Rotation2d.kPi);
    var crossScale =
        new Pose2d(feetToMeters(23.7), feetToMeters(6.8), Rotation2d.fromDegrees(-160));

    var waypoints = new ArrayList<Pose2d>();
    waypoints.add(sideStart);
    waypoints.add(
        sideStart.plus(
            new Transform2d(
                new Translation2d(feetToMeters(-13), feetToMeters(0)), Rotation2d.kZero)));
    waypoints.add(
        sideStart.plus(
            new Transform2d(
                new Translation2d(feetToMeters(-19.5), feetToMeters(5)), Rotation2d.kCW_Pi_2)));
    waypoints.add(crossScale);

    TrajectoryConfig config = new TrajectoryConfig(maxVelocity, maxAccel).setReversed(true);

    return TrajectoryGenerator.generateTrajectory(waypoints, config);
  }

  /**
   * Generates a simple straight-line trajectory.
   *
   * @return A simple trajectory for testing.
   */
  static Trajectory<SplineSample> generateStraightTrajectory() {
    var waypoints =
        List.of(
            new Pose2d(0, 0, Rotation2d.kZero),
            new Pose2d(1, 0, Rotation2d.kZero),
            new Pose2d(2, 0, Rotation2d.kZero));

    TrajectoryConfig config = new TrajectoryConfig(2.0, 2.0);

    return TrajectoryGenerator.generateTrajectory(waypoints, config);
  }

  /**
   * Generates a circular trajectory.
   *
   * @return A circular trajectory for testing.
   */
  static Trajectory<SplineSample> generateCircularTrajectory() {
    var waypoints =
        List.of(
            new Pose2d(1, 0, Rotation2d.kCCW_Pi_2),
            new Pose2d(0, 1, Rotation2d.kPi),
            new Pose2d(-1, 0, Rotation2d.kCW_Pi_2),
            new Pose2d(0, -1, Rotation2d.kZero),
            new Pose2d(1, 0, Rotation2d.kCCW_Pi_2));

    TrajectoryConfig config = new TrajectoryConfig(2.0, 2.0);

    return TrajectoryGenerator.generateTrajectory(waypoints, config);
  }

  @Test
  void testDifferentialTrajectoryConversionCrossScale() {
    // Generate base trajectory
    Trajectory<SplineSample> baseTrajectory = generateTestTrajectory();

    // Create differential drive kinematics with 0.6m trackwidth
    DifferentialDriveKinematics kinematics = new DifferentialDriveKinematics(0.6);

    // Convert to differential trajectory
    DifferentialTrajectory diffTrajectory =
        new DifferentialTrajectory(kinematics, baseTrajectory.samples);

    // Verify all samples have correct wheel speeds
    for (DifferentialSample sample : diffTrajectory.samples) {
      // Calculate expected wheel speeds from chassis speeds using kinematics
      DifferentialDriveWheelSpeeds expectedWheelSpeeds = kinematics.toWheelSpeeds(sample.velocity);

      // Assert the sample's wheel speeds match the kinematics calculation
      assertAll(
          () ->
              assertEquals(
                  expectedWheelSpeeds.left,
                  sample.leftSpeed,
                  kEpsilon,
                  "Left wheel speed mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  expectedWheelSpeeds.right,
                  sample.rightSpeed,
                  kEpsilon,
                  "Right wheel speed mismatch at t=" + sample.timestamp.in(Seconds)));

      // Verify inverse: wheel speeds should produce the original chassis speeds
      ChassisSpeeds reconstructedSpeeds =
          kinematics.toChassisSpeeds(
              new DifferentialDriveWheelSpeeds(sample.leftSpeed, sample.rightSpeed));

      assertAll(
          () ->
              assertEquals(
                  sample.velocity.vx,
                  reconstructedSpeeds.vx,
                  kEpsilon,
                  "Reconstructed vx mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.vy,
                  reconstructedSpeeds.vy,
                  kEpsilon,
                  "Reconstructed vy mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.omega,
                  reconstructedSpeeds.omega,
                  kEpsilon,
                  "Reconstructed omega mismatch at t=" + sample.timestamp.in(Seconds)));
    }
  }

  @Test
  void testDifferentialTrajectoryConversionStraight() {
    Trajectory<SplineSample> baseTrajectory = generateStraightTrajectory();
    DifferentialDriveKinematics kinematics = new DifferentialDriveKinematics(0.7);

    DifferentialTrajectory diffTrajectory =
        new DifferentialTrajectory(kinematics, baseTrajectory.samples);

    for (DifferentialSample sample : diffTrajectory.samples) {
      DifferentialDriveWheelSpeeds expectedWheelSpeeds = kinematics.toWheelSpeeds(sample.velocity);

      assertEquals(expectedWheelSpeeds.left, sample.leftSpeed, kEpsilon);
      assertEquals(expectedWheelSpeeds.right, sample.rightSpeed, kEpsilon);

      // For straight motion, left and right speeds should be approximately equal
      assertEquals(sample.leftSpeed, sample.rightSpeed, 0.01);
    }
  }

  @Test
  void testSwerveTrajectoryConversionCrossScale() {
    // Generate base trajectory
    Trajectory<SplineSample> baseTrajectory = generateTestTrajectory();

    // Create swerve drive kinematics (square configuration)
    Translation2d frontLeft = new Translation2d(0.381, 0.381); // 15 inches in meters
    Translation2d frontRight = new Translation2d(0.381, -0.381);
    Translation2d rearLeft = new Translation2d(-0.381, 0.381);
    Translation2d rearRight = new Translation2d(-0.381, -0.381);
    SwerveDriveKinematics kinematics =
        new SwerveDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);

    // Convert to swerve trajectory
    SwerveTrajectory swerveTrajectory = new SwerveTrajectory(kinematics, baseTrajectory.samples);

    // Verify all samples have correct module states
    for (SwerveSample sample : swerveTrajectory.samples) {
      // Calculate expected module states from chassis speeds using kinematics
      SwerveModuleState[] expectedStates = kinematics.toWheelSpeeds(sample.velocity);

      // Assert the sample's module states match the kinematics calculation
      assertEquals(
          expectedStates.length,
          sample.states.length,
          "Module state count mismatch at t=" + sample.timestamp.in(Seconds));

      for (int i = 0; i < expectedStates.length; i++) {
        final int moduleIndex = i;
        assertAll(
            () ->
                assertEquals(
                    expectedStates[moduleIndex].speed,
                    sample.states[moduleIndex].speed,
                    kEpsilon,
                    "Module "
                        + moduleIndex
                        + " speed mismatch at t="
                        + sample.timestamp.in(Seconds)),
            () -> {
              // Only compare angles when module speed is significant
              // When speed is near zero, angle is not well-defined
              if (Math.abs(expectedStates[moduleIndex].speed) > 0.01) {
                assertEquals(
                    expectedStates[moduleIndex].angle.getRadians(),
                    sample.states[moduleIndex].angle.getRadians(),
                    0.01,
                    "Module "
                        + moduleIndex
                        + " angle mismatch at t="
                        + sample.timestamp.in(Seconds));
              }
            });
      }

      // Verify inverse: module states should produce the original chassis speeds
      ChassisSpeeds reconstructedSpeeds = kinematics.toChassisSpeeds(sample.states);

      assertAll(
          () ->
              assertEquals(
                  sample.velocity.vx,
                  reconstructedSpeeds.vx,
                  0.001,
                  "Reconstructed vx mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.vy,
                  reconstructedSpeeds.vy,
                  0.001,
                  "Reconstructed vy mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.omega,
                  reconstructedSpeeds.omega,
                  0.001,
                  "Reconstructed omega mismatch at t=" + sample.timestamp.in(Seconds)));
    }
  }

  @Test
  void testSwerveTrajectoryConversionCircular() {
    Trajectory<SplineSample> baseTrajectory = generateCircularTrajectory();

    Translation2d frontLeft = new Translation2d(0.3, 0.3);
    Translation2d frontRight = new Translation2d(0.3, -0.3);
    Translation2d rearLeft = new Translation2d(-0.3, 0.3);
    Translation2d rearRight = new Translation2d(-0.3, -0.3);
    SwerveDriveKinematics kinematics =
        new SwerveDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);

    SwerveTrajectory swerveTrajectory = new SwerveTrajectory(kinematics, baseTrajectory.samples);

    for (SwerveSample sample : swerveTrajectory.samples) {
      SwerveModuleState[] expectedStates = kinematics.toWheelSpeeds(sample.velocity);

      assertEquals(expectedStates.length, sample.states.length);

      for (int i = 0; i < expectedStates.length; i++) {
        assertEquals(expectedStates[i].speed, sample.states[i].speed, kEpsilon);
        // Only compare angles when module speed is significant
        if (Math.abs(expectedStates[i].speed) > 0.01) {
          assertEquals(
              expectedStates[i].angle.getRadians(), sample.states[i].angle.getRadians(), 0.01);
        }
      }
    }
  }

  @Test
  void testMecanumTrajectoryConversionCrossScale() {
    // Generate base trajectory
    Trajectory<SplineSample> baseTrajectory = generateTestTrajectory();

    // Create mecanum drive kinematics
    Translation2d frontLeft = new Translation2d(0.381, 0.381);
    Translation2d frontRight = new Translation2d(0.381, -0.381);
    Translation2d rearLeft = new Translation2d(-0.381, 0.381);
    Translation2d rearRight = new Translation2d(-0.381, -0.381);
    MecanumDriveKinematics kinematics =
        new MecanumDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);

    // Convert to mecanum trajectory
    MecanumTrajectory mecanumTrajectory = new MecanumTrajectory(kinematics, baseTrajectory.samples);

    // Verify all samples have correct wheel speeds
    for (MecanumSample sample : mecanumTrajectory.samples) {
      // Calculate expected wheel speeds from chassis speeds using kinematics
      MecanumDriveWheelSpeeds expectedWheelSpeeds = kinematics.toWheelSpeeds(sample.velocity);

      // Assert the sample's wheel speeds match the kinematics calculation
      assertAll(
          () ->
              assertEquals(
                  expectedWheelSpeeds.frontLeft,
                  sample.speeds.frontLeft,
                  kEpsilon,
                  "Front left wheel speed mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  expectedWheelSpeeds.frontRight,
                  sample.speeds.frontRight,
                  kEpsilon,
                  "Front right wheel speed mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  expectedWheelSpeeds.rearLeft,
                  sample.speeds.rearLeft,
                  kEpsilon,
                  "Rear left wheel speed mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  expectedWheelSpeeds.rearRight,
                  sample.speeds.rearRight,
                  kEpsilon,
                  "Rear right wheel speed mismatch at t=" + sample.timestamp.in(Seconds)));

      // Verify inverse: wheel speeds should produce the original chassis speeds
      ChassisSpeeds reconstructedSpeeds = kinematics.toChassisSpeeds(sample.speeds);

      assertAll(
          () ->
              assertEquals(
                  sample.velocity.vx,
                  reconstructedSpeeds.vx,
                  0.001,
                  "Reconstructed vx mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.vy,
                  reconstructedSpeeds.vy,
                  0.001,
                  "Reconstructed vy mismatch at t=" + sample.timestamp.in(Seconds)),
          () ->
              assertEquals(
                  sample.velocity.omega,
                  reconstructedSpeeds.omega,
                  0.001,
                  "Reconstructed omega mismatch at t=" + sample.timestamp.in(Seconds)));
    }
  }

  @Test
  void testMecanumTrajectoryConversionStraight() {
    Trajectory<SplineSample> baseTrajectory = generateStraightTrajectory();

    Translation2d frontLeft = new Translation2d(0.25, 0.25);
    Translation2d frontRight = new Translation2d(0.25, -0.25);
    Translation2d rearLeft = new Translation2d(-0.25, 0.25);
    Translation2d rearRight = new Translation2d(-0.25, -0.25);
    MecanumDriveKinematics kinematics =
        new MecanumDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);

    MecanumTrajectory mecanumTrajectory = new MecanumTrajectory(kinematics, baseTrajectory.samples);

    for (MecanumSample sample : mecanumTrajectory.samples) {
      MecanumDriveWheelSpeeds expectedWheelSpeeds = kinematics.toWheelSpeeds(sample.velocity);

      assertEquals(expectedWheelSpeeds.frontLeft, sample.speeds.frontLeft, kEpsilon);
      assertEquals(expectedWheelSpeeds.frontRight, sample.speeds.frontRight, kEpsilon);
      assertEquals(expectedWheelSpeeds.rearLeft, sample.speeds.rearLeft, kEpsilon);
      assertEquals(expectedWheelSpeeds.rearRight, sample.speeds.rearRight, kEpsilon);
    }
  }

  @Test
  void testMultipleDrivetrainsSameBaseTrajectory() {
    // Generate a single base trajectory and convert it to all three drivetrain types
    Trajectory<SplineSample> baseTrajectory = generateCircularTrajectory();

    // Differential drive
    DifferentialDriveKinematics diffKinematics = new DifferentialDriveKinematics(0.6);
    DifferentialTrajectory diffTrajectory =
        new DifferentialTrajectory(diffKinematics, baseTrajectory.samples);

    // Swerve drive
    Translation2d frontLeft = new Translation2d(0.3, 0.3);
    Translation2d frontRight = new Translation2d(0.3, -0.3);
    Translation2d rearLeft = new Translation2d(-0.3, 0.3);
    Translation2d rearRight = new Translation2d(-0.3, -0.3);
    SwerveDriveKinematics swerveKinematics =
        new SwerveDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);
    SwerveTrajectory swerveTrajectory =
        new SwerveTrajectory(swerveKinematics, baseTrajectory.samples);

    // Mecanum drive
    MecanumDriveKinematics mecanumKinematics =
        new MecanumDriveKinematics(frontLeft, frontRight, rearLeft, rearRight);
    MecanumTrajectory mecanumTrajectory =
        new MecanumTrajectory(mecanumKinematics, baseTrajectory.samples);

    // Verify all trajectories have the same number of samples
    assertEquals(baseTrajectory.samples.length, diffTrajectory.samples.length);
    assertEquals(baseTrajectory.samples.length, swerveTrajectory.samples.length);
    assertEquals(baseTrajectory.samples.length, mecanumTrajectory.samples.length);

    // Verify each drivetrain-specific trajectory has correct kinematics
    for (int i = 0; i < baseTrajectory.samples.length; i++) {
      SplineSample baseSample = baseTrajectory.samples[i];
      DifferentialSample diffSample = diffTrajectory.samples[i];
      SwerveSample swerveSample = swerveTrajectory.samples[i];
      MecanumSample mecanumSample = mecanumTrajectory.samples[i];

      // Verify poses and velocities match
      assertEquals(baseSample.pose.getX(), diffSample.pose.getX(), kEpsilon);
      assertEquals(baseSample.pose.getX(), swerveSample.pose.getX(), kEpsilon);
      assertEquals(baseSample.pose.getX(), mecanumSample.pose.getX(), kEpsilon);

      // Verify kinematics for each type
      DifferentialDriveWheelSpeeds expectedDiffSpeeds =
          diffKinematics.toWheelSpeeds(diffSample.velocity);
      assertEquals(expectedDiffSpeeds.left, diffSample.leftSpeed, kEpsilon);
      assertEquals(expectedDiffSpeeds.right, diffSample.rightSpeed, kEpsilon);

      SwerveModuleState[] expectedSwerveSpeeds =
          swerveKinematics.toWheelSpeeds(swerveSample.velocity);
      for (int j = 0; j < expectedSwerveSpeeds.length; j++) {
        assertEquals(expectedSwerveSpeeds[j].speed, swerveSample.states[j].speed, kEpsilon);
      }

      MecanumDriveWheelSpeeds expectedMecanumSpeeds =
          mecanumKinematics.toWheelSpeeds(mecanumSample.velocity);
      assertEquals(expectedMecanumSpeeds.frontLeft, mecanumSample.speeds.frontLeft, kEpsilon);
      assertEquals(expectedMecanumSpeeds.frontRight, mecanumSample.speeds.frontRight, kEpsilon);
      assertEquals(expectedMecanumSpeeds.rearLeft, mecanumSample.speeds.rearLeft, kEpsilon);
      assertEquals(expectedMecanumSpeeds.rearRight, mecanumSample.speeds.rearRight, kEpsilon);
    }
  }
}
