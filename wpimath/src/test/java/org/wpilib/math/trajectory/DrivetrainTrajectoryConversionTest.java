// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.util.Units.feetToMeters;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.DifferentialDriveWheelVelocities;

class DrivetrainTrajectoryConversionTest {
  private static final double kEpsilon = 1e-6;

  /**
   * Generates a sample trajectory similar to the 2018 cross scale auto.
   *
   * @return A spline trajectory for testing.
   */
  static Trajectory<DrivetrainSplineSample> generateTestTrajectory() {
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

    return DrivetrainSplineTrajectoryGenerator.generate(waypoints, config);
  }

  /**
   * Generates a simple straight-line trajectory.
   *
   * @return A simple trajectory for testing.
   */
  static Trajectory<DrivetrainSplineSample> generateStraightTrajectory() {
    var waypoints =
        List.of(
            new Pose2d(0, 0, Rotation2d.kZero),
            new Pose2d(1, 0, Rotation2d.kZero),
            new Pose2d(2, 0, Rotation2d.kZero));

    TrajectoryConfig config = new TrajectoryConfig(2.0, 2.0);

    return DrivetrainSplineTrajectoryGenerator.generate(waypoints, config);
  }

  /**
   * Generates a circular trajectory.
   *
   * @return A circular trajectory for testing.
   */
  static Trajectory<DrivetrainSplineSample> generateCircularTrajectory() {
    var waypoints =
        List.of(
            new Pose2d(1, 0, Rotation2d.kCCW_Pi_2),
            new Pose2d(0, 1, Rotation2d.kPi),
            new Pose2d(-1, 0, Rotation2d.kCW_Pi_2),
            new Pose2d(0, -1, Rotation2d.kZero),
            new Pose2d(1, 0, Rotation2d.kCCW_Pi_2));

    TrajectoryConfig config = new TrajectoryConfig(2.0, 2.0);

    return DrivetrainSplineTrajectoryGenerator.generate(waypoints, config);
  }

  @Test
  void testDifferentialTrajectoryConversionCrossScale() {
    // Generate base trajectory
    Trajectory<DrivetrainSplineSample> baseTrajectory = generateTestTrajectory();

    // Create differential drive kinematics with 0.6m trackwidth
    DifferentialDriveKinematics kinematics = new DifferentialDriveKinematics(0.6);

    // Convert to differential trajectory
    DifferentialTrajectory diffTrajectory =
        new DifferentialTrajectory(kinematics, baseTrajectory.samples);

    // Verify all samples have correct wheel speeds
    for (DifferentialSample sample : diffTrajectory.samples) {
      // Calculate expected wheel speeds from chassis speeds using kinematics
      DifferentialDriveWheelVelocities expectedWheelVelocities =
          kinematics.toWheelVelocities(sample.velocity.toRobotRelative(sample.pose.getRotation()));

      // Assert the sample's wheel speeds match the kinematics calculation
      assertAll(
          () ->
              assertEquals(
                  expectedWheelVelocities.left,
                  sample.leftVelocity,
                  kEpsilon,
                  "Left wheel speed mismatch at t=" + sample.time),
          () ->
              assertEquals(
                  expectedWheelVelocities.right,
                  sample.rightVelocity,
                  kEpsilon,
                  "Right wheel speed mismatch at t=" + sample.time));

      // Verify inverse: wheel speeds should produce the original chassis speeds
      ChassisVelocities reconstructedVelocities =
          kinematics.toChassisVelocities(
              new DifferentialDriveWheelVelocities(sample.leftVelocity, sample.rightVelocity));

      assertAll(
          () ->
              assertEquals(
                  sample.velocity.toRobotRelative(sample.pose.getRotation()).vx,
                  reconstructedVelocities.vx,
                  kEpsilon,
                  "Reconstructed vx mismatch at t=" + sample.time),
          () ->
              assertEquals(
                  sample.velocity.toRobotRelative(sample.pose.getRotation()).vy,
                  reconstructedVelocities.vy,
                  kEpsilon,
                  "Reconstructed vy mismatch at t=" + sample.time),
          () ->
              assertEquals(
                  sample.velocity.toRobotRelative(sample.pose.getRotation()).omega,
                  reconstructedVelocities.omega,
                  kEpsilon,
                  "Reconstructed omega mismatch at t=" + sample.time));
    }
  }

  @Test
  void testDifferentialTrajectoryConversionStraight() {
    Trajectory<DrivetrainSplineSample> baseTrajectory = generateStraightTrajectory();
    DifferentialDriveKinematics kinematics = new DifferentialDriveKinematics(0.7);

    DifferentialTrajectory diffTrajectory =
        new DifferentialTrajectory(kinematics, baseTrajectory.samples);

    for (DifferentialSample sample : diffTrajectory.samples) {
      DifferentialDriveWheelVelocities expectedWheelVelocities =
          kinematics.toWheelVelocities(sample.velocity.toRobotRelative(sample.pose.getRotation()));

      assertEquals(expectedWheelVelocities.left, sample.leftVelocity, kEpsilon);
      assertEquals(expectedWheelVelocities.right, sample.rightVelocity, kEpsilon);

      // For straight motion, left and right speeds should be approximately equal
      assertEquals(sample.leftVelocity, sample.rightVelocity, 0.01);
    }
  }
}
