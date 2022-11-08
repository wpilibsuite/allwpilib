// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Random;
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
        new SwerveDrivePoseEstimator<N7, N7, N5>(
            Nat.N7(),
            Nat.N7(),
            Nat.N5(),
            new Rotation2d(),
            new Pose2d(),
            new SwerveModulePosition[] {fl, fr, bl, br},
            kinematics,
            VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1),
            VecBuilder.fill(0.005, 0.005, 0.005, 0.005, 0.005),
            VecBuilder.fill(0.1, 0.1, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(4915);

    final double dt = 0.02;
    double t = 0.0;

    final double visionUpdateRate = 0.1;
    Pose2d lastVisionPose = null;
    double lastVisionUpdateTime = Double.NEGATIVE_INFINITY;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      if (lastVisionUpdateTime + visionUpdateRate < t) {
        if (lastVisionPose != null) {
          estimator.addVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
        }

        lastVisionPose =
            new Pose2d(
                new Translation2d(
                    groundTruthState.poseMeters.getTranslation().getX() + rand.nextGaussian() * 0.1,
                    groundTruthState.poseMeters.getTranslation().getY()
                        + rand.nextGaussian() * 0.1),
                new Rotation2d(rand.nextGaussian() * 0.1)
                    .plus(groundTruthState.poseMeters.getRotation()));

        lastVisionUpdateTime = t;
      }

      var moduleStates =
          kinematics.toSwerveModuleStates(
              new ChassisSpeeds(
                  groundTruthState.velocityMetersPerSecond,
                  0.0,
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.curvatureRadPerMeter));
      for (var moduleState : moduleStates) {
        moduleState.angle = moduleState.angle.plus(new Rotation2d(rand.nextGaussian() * 0.005));
        moduleState.speedMetersPerSecond += rand.nextGaussian() * 0.1;
      }

      fl.distanceMeters += moduleStates[0].speedMetersPerSecond * dt;
      fr.distanceMeters += moduleStates[1].speedMetersPerSecond * dt;
      bl.distanceMeters += moduleStates[2].speedMetersPerSecond * dt;
      br.distanceMeters += moduleStates[3].speedMetersPerSecond * dt;

      fl.angle = moduleStates[0].angle;
      fr.angle = moduleStates[1].angle;
      bl.angle = moduleStates[2].angle;
      br.angle = moduleStates[3].angle;

      var xHat =
          estimator.updateWithTime(
              t,
              groundTruthState
                  .poseMeters
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.05)),
              moduleStates,
              new SwerveModulePosition[] {fl, fr, bl, br});

      double error =
          groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.05, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.125, "Incorrect max error");
  }

  @Test
  void testAccuracyFacingXAxis() {
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
        new SwerveDrivePoseEstimator<N7, N7, N5>(
            Nat.N7(),
            Nat.N7(),
            Nat.N5(),
            new Rotation2d(),
            new Pose2d(),
            new SwerveModulePosition[] {fl, fr, bl, br},
            kinematics,
            VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1),
            VecBuilder.fill(0.005, 0.005, 0.005, 0.005, 0.005),
            VecBuilder.fill(0.1, 0.1, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                new Pose2d(0, 0, Rotation2d.fromDegrees(45))),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(4915);

    final double dt = 0.02;
    double t = 0.0;

    final double visionUpdateRate = 0.1;
    Pose2d lastVisionPose = null;
    double lastVisionUpdateTime = Double.NEGATIVE_INFINITY;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= trajectory.getTotalTimeSeconds()) {
      var groundTruthState = trajectory.sample(t);

      if (lastVisionUpdateTime + visionUpdateRate < t) {
        if (lastVisionPose != null) {
          estimator.addVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
        }

        lastVisionPose =
            new Pose2d(
                new Translation2d(
                    groundTruthState.poseMeters.getTranslation().getX() + rand.nextGaussian() * 0.1,
                    groundTruthState.poseMeters.getTranslation().getY()
                        + rand.nextGaussian() * 0.1),
                new Rotation2d(rand.nextGaussian() * 0.1)
                    .plus(groundTruthState.poseMeters.getRotation()));

        lastVisionUpdateTime = t;
      }

      var moduleStates =
          kinematics.toSwerveModuleStates(
              new ChassisSpeeds(
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.poseMeters.getRotation().getCos(),
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.poseMeters.getRotation().getSin(),
                  0.0));
      for (var moduleState : moduleStates) {
        moduleState.angle = moduleState.angle.plus(new Rotation2d(rand.nextGaussian() * 0.005));
        moduleState.speedMetersPerSecond += rand.nextGaussian() * 0.1;
      }

      fl.distanceMeters +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;
      fr.distanceMeters +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;
      bl.distanceMeters +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;
      br.distanceMeters +=
          groundTruthState.velocityMetersPerSecond * dt
              + 0.5 * groundTruthState.accelerationMetersPerSecondSq * dt * dt;

      fl.angle = groundTruthState.poseMeters.getRotation();
      fr.angle = groundTruthState.poseMeters.getRotation();
      bl.angle = groundTruthState.poseMeters.getRotation();
      br.angle = groundTruthState.poseMeters.getRotation();

      var xHat =
          estimator.updateWithTime(
              t,
              new Rotation2d(rand.nextGaussian() * 0.05),
              moduleStates,
              new SwerveModulePosition[] {fl, fr, bl, br});

      double error =
          groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
        0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.05, "Incorrect mean error");
    assertEquals(0.0, maxError, 0.125, "Incorrect max error");
  }
}
