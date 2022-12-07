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
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Random;
import java.util.TreeMap;
import java.util.function.Function;
import org.junit.jupiter.api.Test;

class SwerveDrivePoseEstimatorTest {
  @Test
  void testAccuracyFacingTrajectory() {
    var kinematics =
        new SwerveDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1),
            new Translation2d(-1, 1));

    var fl = new SwerveModulePosition();
    var fr = new SwerveModulePosition();
    var bl = new SwerveModulePosition();
    var br = new SwerveModulePosition();

    var estimator =
        new SwerveDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            new SwerveModulePosition[] {fl, fr, bl, br},
            new Pose2d(),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.5, 0.5, 0.5));

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
        new SwerveDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1),
            new Translation2d(-1, 1));

    var fl = new SwerveModulePosition();
    var fr = new SwerveModulePosition();
    var bl = new SwerveModulePosition();
    var br = new SwerveModulePosition();

    var estimator =
        new SwerveDrivePoseEstimator(
            kinematics,
            new Rotation2d(),
            new SwerveModulePosition[] {fl, fr, bl, br},
            new Pose2d(-1, -1, Rotation2d.fromRadians(-1)),
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.9, 0.9, 0.9));
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
            1.0,
            false);
      }
    }
  }

  void testFollowTrajectory(
      final SwerveDriveKinematics kinematics,
      final SwerveDrivePoseEstimator estimator,
      final Trajectory trajectory,
      final Function<Trajectory.State, ChassisSpeeds> chassisSpeedsGenerator,
      final Function<Trajectory.State, Pose2d> visionMeasurementGenerator,
      final Pose2d startingPose,
      final Pose2d endingPose,
      final double dt,
      final double visionUpdateRate,
      final double visionUpdateDelay,
      final boolean checkError) {
    SwerveModulePosition[] positions = {
      new SwerveModulePosition(),
      new SwerveModulePosition(),
      new SwerveModulePosition(),
      new SwerveModulePosition()
    };

    estimator.resetPosition(new Rotation2d(), positions, startingPose);

    var rand = new Random(3538);

    double t = 0.0;

    System.out.print(
        "time, est_x, est_y, est_theta, true_x, true_y, true_theta, "
            + "distance_1, distance_2, distance_3, distance_4, "
            + "angle_1, angle_2, angle_3, angle_4\n");

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

      var moduleStates = kinematics.toSwerveModuleStates(chassisSpeeds);

      for (int i = 0; i < moduleStates.length; i++) {
        positions[i].distanceMeters +=
            moduleStates[i].speedMetersPerSecond * (1 - rand.nextGaussian() * 0.05) * dt;
        positions[i].angle =
            moduleStates[i].angle.plus(new Rotation2d(rand.nextGaussian() * 0.005));
      }

      var xHat =
          estimator.updateWithTime(
              t,
              groundTruthState
                  .poseMeters
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.05))
                  .minus(trajectory.getInitialPose().getRotation()),
              positions);

      System.out.printf(
          "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
          t,
          xHat.getX(),
          xHat.getY(),
          xHat.getRotation().getRadians(),
          groundTruthState.poseMeters.getX(),
          groundTruthState.poseMeters.getY(),
          groundTruthState.poseMeters.getRotation().getRadians(),
          positions[0].distanceMeters,
          positions[1].distanceMeters,
          positions[2].distanceMeters,
          positions[3].distanceMeters,
          positions[0].angle.getRadians(),
          positions[1].angle.getRadians(),
          positions[2].angle.getRadians(),
          positions[3].angle.getRadians());

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
