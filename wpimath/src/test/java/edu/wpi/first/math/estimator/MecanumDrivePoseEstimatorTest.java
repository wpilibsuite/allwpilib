// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveWheelPositions;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Optional;
import java.util.Random;
import java.util.TreeMap;
import java.util.function.Function;
import org.junit.jupiter.api.Test;

class MecanumDrivePoseEstimatorTest {
  private static final double kEpsilon = 1e-9;

  @Test
  void testAccuracyFacingTrajectory() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var wheelPositions = new MecanumDriveWheelPositions();

    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            wheelPositions,
            Pose2d.kZero,
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.45, 0.45, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(2, 2));

    testFollowTrajectory(
        kinematics,
        estimator,
        trajectory,
        state ->
            new ChassisSpeeds(
                state.velocityMetersPerSecond,
                0,
                state.velocityMetersPerSecond * state.curvatureRadPerMeter),
        state -> state.poseMeters,
        trajectory.getInitialPose(),
        new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
        0.02,
        0.1,
        0.25,
        true);
  }

  @Test
  void testBadInitialPose() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var wheelPositions = new MecanumDriveWheelPositions();

    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            wheelPositions,
            Pose2d.kZero,
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.45, 0.45, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.kCW_Pi_2),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(2, 2));

    for (int offset_direction_degs = 0; offset_direction_degs < 360; offset_direction_degs += 45) {
      for (int offset_heading_degs = 0; offset_heading_degs < 360; offset_heading_degs += 45) {
        var pose_offset = Rotation2d.fromDegrees(offset_direction_degs);
        var heading_offset = Rotation2d.fromDegrees(offset_heading_degs);

        var initial_pose =
            trajectory
                .getInitialPose()
                .plus(
                    new Transform2d(
                        new Translation2d(pose_offset.getCos(), pose_offset.getSin()),
                        heading_offset));

        testFollowTrajectory(
            kinematics,
            estimator,
            trajectory,
            state ->
                new ChassisSpeeds(
                    state.velocityMetersPerSecond,
                    0,
                    state.velocityMetersPerSecond * state.curvatureRadPerMeter),
            state -> state.poseMeters,
            initial_pose,
            new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
            0.02,
            0.1,
            0.25,
            false);
      }
    }
  }

  void testFollowTrajectory(
      final MecanumDriveKinematics kinematics,
      final MecanumDrivePoseEstimator estimator,
      final Trajectory trajectory,
      final Function<Trajectory.State, ChassisSpeeds> chassisSpeedsGenerator,
      final Function<Trajectory.State, Pose2d> visionMeasurementGenerator,
      final Pose2d startingPose,
      final Pose2d endingPose,
      final double dt,
      final double visionUpdateRate,
      final double visionUpdateDelay,
      final boolean checkError) {
    var wheelPositions = new MecanumDriveWheelPositions();

    estimator.resetPosition(Rotation2d.kZero, wheelPositions, startingPose);

    var rand = new Random(3538);

    double t = 0.0;

    final TreeMap<Double, Pose2d> visionUpdateQueue = new TreeMap<>();

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      // We are due for a new vision measurement if it's been `visionUpdateRate` seconds since the
      // last vision measurement
      if (visionUpdateQueue.isEmpty() || visionUpdateQueue.lastKey() + visionUpdateRate < t) {
        Pose2d newVisionPose =
            visionMeasurementGenerator
                .apply(groundTruthState)
                .plus(
                    new Transform2d(
                        new Translation2d(rand.nextGaussian() * 0.1, rand.nextGaussian() * 0.1),
                        new Rotation2d(rand.nextGaussian() * 0.05)));

        visionUpdateQueue.put(t, newVisionPose);
      }

      // We should apply the oldest vision measurement if it has been `visionUpdateDelay` seconds
      // since it was measured
      if (!visionUpdateQueue.isEmpty() && visionUpdateQueue.firstKey() + visionUpdateDelay < t) {
        var visionEntry = visionUpdateQueue.pollFirstEntry();
        estimator.addVisionMeasurement(visionEntry.getValue(), visionEntry.getKey());
      }

      var chassisSpeeds = chassisSpeedsGenerator.apply(groundTruthState);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      wheelPositions.frontLeftMeters += wheelSpeeds.frontLeftMetersPerSecond * dt;
      wheelPositions.frontRightMeters += wheelSpeeds.frontRightMetersPerSecond * dt;
      wheelPositions.rearLeftMeters += wheelSpeeds.rearLeftMetersPerSecond * dt;
      wheelPositions.rearRightMeters += wheelSpeeds.rearRightMetersPerSecond * dt;

      var xHat =
          estimator.updateWithTime(
              t,
              groundTruthState
                  .poseMeters
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.05))
                  .minus(trajectory.getInitialPose().getRotation()),
              wheelPositions);

      double error =
          groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        endingPose.getX(), estimator.getEstimatedPosition().getX(), 0.08, "Incorrect Final X");
    assertEquals(
        endingPose.getY(), estimator.getEstimatedPosition().getY(), 0.08, "Incorrect Final Y");
    assertEquals(
        endingPose.getRotation().getRadians(),
        estimator.getEstimatedPosition().getRotation().getRadians(),
        0.15,
        "Incorrect Final Theta");

    if (checkError) {
      assertEquals(
          0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.07, "Incorrect mean error");
      assertEquals(0.0, maxError, 0.2, "Incorrect max error");
    }
  }

  @Test
  void testSimultaneousVisionMeasurements() {
    // This tests for multiple vision measurements applied at the same time. The expected behavior
    // is that all measurements affect the estimated pose. The alternative result is that only one
    // vision measurement affects the outcome. If that were the case, after 1000 measurements, the
    // estimated pose would converge to that measurement.
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var wheelPositions = new MecanumDriveWheelPositions();

    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            wheelPositions,
            new Pose2d(1, 2, Rotation2d.kCW_Pi_2),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.45, 0.45, 0.1));

    estimator.updateWithTime(0, Rotation2d.kZero, wheelPositions);

    var visionMeasurements =
        new Pose2d[] {
          new Pose2d(0, 0, Rotation2d.kZero),
          new Pose2d(3, 1, Rotation2d.kCCW_Pi_2),
          new Pose2d(2, 4, Rotation2d.kPi),
        };

    for (int i = 0; i < 1000; i++) {
      for (var measurement : visionMeasurements) {
        estimator.addVisionMeasurement(measurement, 0);
      }
    }

    for (var measurement : visionMeasurements) {
      var errorLog =
          "Estimator converged to one vision measurement: "
              + estimator.getEstimatedPosition().toString()
              + " -> "
              + measurement;

      var dx = Math.abs(measurement.getX() - estimator.getEstimatedPosition().getX());
      var dy = Math.abs(measurement.getY() - estimator.getEstimatedPosition().getY());
      var dtheta =
          Math.abs(
              measurement.getRotation().getDegrees()
                  - estimator.getEstimatedPosition().getRotation().getDegrees());

      assertTrue(dx > 0.08 || dy > 0.08 || dtheta > 0.08, errorLog);
    }
  }

  @Test
  void testDiscardsOldVisionMeasurements() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(-1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1));
    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            new MecanumDriveWheelPositions(),
            Pose2d.kZero,
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.9, 0.9, 0.9));

    double time = 0;

    // Add enough measurements to fill up the buffer
    for (; time < 4; time += 0.02) {
      estimator.updateWithTime(time, Rotation2d.kZero, new MecanumDriveWheelPositions());
    }

    var odometryPose = estimator.getEstimatedPosition();

    // Apply a vision measurement made 3 seconds ago
    // This test passes if this does not cause a ConcurrentModificationException.
    estimator.addVisionMeasurement(
        new Pose2d(new Translation2d(10, 10), new Rotation2d(0.1)),
        1,
        VecBuilder.fill(0.1, 0.1, 0.1));

    assertEquals(odometryPose.getX(), estimator.getEstimatedPosition().getX(), "Incorrect Final X");
    assertEquals(odometryPose.getY(), estimator.getEstimatedPosition().getY(), "Incorrect Final Y");
    assertEquals(
        odometryPose.getRotation().getRadians(),
        estimator.getEstimatedPosition().getRotation().getRadians(),
        "Incorrect Final Theta");
  }

  @Test
  void testSampleAt() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(-1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1));
    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            new MecanumDriveWheelPositions(),
            Pose2d.kZero,
            VecBuilder.fill(1, 1, 1),
            VecBuilder.fill(1, 1, 1));

    // Returns empty when null
    assertEquals(Optional.empty(), estimator.sampleAt(1));

    // Add odometry measurements, but don't fill up the buffer
    // Add a tiny tolerance for the upper bound because of floating point rounding error
    for (double time = 1; time <= 2 + 1e-9; time += 0.02) {
      var wheelPositions = new MecanumDriveWheelPositions(time, time, time, time);
      estimator.updateWithTime(time, Rotation2d.kZero, wheelPositions);
    }

    // Sample at an added time
    assertEquals(Optional.of(new Pose2d(1.02, 0, Rotation2d.kZero)), estimator.sampleAt(1.02));
    // Sample between updates (test interpolation)
    assertEquals(Optional.of(new Pose2d(1.01, 0, Rotation2d.kZero)), estimator.sampleAt(1.01));
    // Sampling before the oldest value returns the oldest value
    assertEquals(Optional.of(new Pose2d(1, 0, Rotation2d.kZero)), estimator.sampleAt(0.5));
    // Sampling after the newest value returns the newest value
    assertEquals(Optional.of(new Pose2d(2, 0, Rotation2d.kZero)), estimator.sampleAt(2.5));

    // Add a vision measurement after the odometry measurements (while keeping all of the old
    // odometry measurements)
    estimator.addVisionMeasurement(new Pose2d(2, 0, new Rotation2d(1)), 2.2);

    // Make sure nothing changed (except the newest value)
    assertEquals(Optional.of(new Pose2d(1.02, 0, Rotation2d.kZero)), estimator.sampleAt(1.02));
    assertEquals(Optional.of(new Pose2d(1.01, 0, Rotation2d.kZero)), estimator.sampleAt(1.01));
    assertEquals(Optional.of(new Pose2d(1, 0, Rotation2d.kZero)), estimator.sampleAt(0.5));

    // Add a vision measurement before the odometry measurements that's still in the buffer
    estimator.addVisionMeasurement(new Pose2d(1, 0.2, Rotation2d.kZero), 0.9);

    // Everything should be the same except Y is 0.1 (halfway between 0 and 0.2)
    assertEquals(Optional.of(new Pose2d(1.02, 0.1, Rotation2d.kZero)), estimator.sampleAt(1.02));
    assertEquals(Optional.of(new Pose2d(1.01, 0.1, Rotation2d.kZero)), estimator.sampleAt(1.01));
    assertEquals(Optional.of(new Pose2d(1, 0.1, Rotation2d.kZero)), estimator.sampleAt(0.5));
    assertEquals(Optional.of(new Pose2d(2, 0.1, Rotation2d.kZero)), estimator.sampleAt(2.5));
  }

  @Test
  void testReset() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(-1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1));
    var estimator =
        new MecanumDrivePoseEstimator(
            kinematics,
            Rotation2d.kZero,
            new MecanumDriveWheelPositions(),
            Pose2d.kZero,
            VecBuilder.fill(1, 1, 1),
            VecBuilder.fill(1, 1, 1));

    // Test reset position
    estimator.resetPosition(
        Rotation2d.kZero,
        new MecanumDriveWheelPositions(1, 1, 1, 1),
        new Pose2d(1, 0, Rotation2d.kZero));

    assertAll(
        () -> assertEquals(1, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(0, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(0, estimator.getEstimatedPosition().getRotation().getRadians(), kEpsilon));

    // Test orientation and wheel positions
    estimator.update(Rotation2d.kZero, new MecanumDriveWheelPositions(2, 2, 2, 2));

    assertAll(
        () -> assertEquals(2, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(0, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(0, estimator.getEstimatedPosition().getRotation().getRadians(), kEpsilon));

    // Test reset rotation
    estimator.resetRotation(Rotation2d.kCCW_Pi_2);

    assertAll(
        () -> assertEquals(2, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(0, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(
                Math.PI / 2,
                estimator.getEstimatedPosition().getRotation().getRadians(),
                kEpsilon));

    // Test orientation
    estimator.update(Rotation2d.kZero, new MecanumDriveWheelPositions(3, 3, 3, 3));

    assertAll(
        () -> assertEquals(2, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(1, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(
                Math.PI / 2,
                estimator.getEstimatedPosition().getRotation().getRadians(),
                kEpsilon));

    // Test reset translation
    estimator.resetTranslation(new Translation2d(-1, -1));

    assertAll(
        () -> assertEquals(-1, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(-1, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(
                Math.PI / 2,
                estimator.getEstimatedPosition().getRotation().getRadians(),
                kEpsilon));

    // Test reset pose
    estimator.resetPose(Pose2d.kZero);

    assertAll(
        () -> assertEquals(0, estimator.getEstimatedPosition().getX(), kEpsilon),
        () -> assertEquals(0, estimator.getEstimatedPosition().getY(), kEpsilon),
        () ->
            assertEquals(0, estimator.getEstimatedPosition().getRotation().getRadians(), kEpsilon));
  }
}
