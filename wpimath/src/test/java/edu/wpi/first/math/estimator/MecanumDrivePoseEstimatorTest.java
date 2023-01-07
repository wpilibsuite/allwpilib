// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

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
import java.util.Random;
import java.util.TreeMap;
import java.util.function.Function;
import org.junit.jupiter.api.Test;

class MecanumDrivePoseEstimatorTest {
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
            new Rotation2d(),
            wheelPositions,
            new Pose2d(),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.45, 0.45, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
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
            new Rotation2d(),
            wheelPositions,
            new Pose2d(),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.45, 0.45, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
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

    estimator.resetPosition(new Rotation2d(), wheelPositions, startingPose);

    var rand = new Random(3538);

    double t = 0.0;

    System.out.print("time, est_x, est_y, est_theta, true_x, true_y, true_theta\n");

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

      System.out.printf(
          "%f, %f, %f, %f, %f, %f, %f\n",
          t,
          xHat.getX(),
          xHat.getY(),
          xHat.getRotation().getRadians(),
          groundTruthState.poseMeters.getX(),
          groundTruthState.poseMeters.getY(),
          groundTruthState.poseMeters.getRotation().getRadians());

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
}
