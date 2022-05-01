// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import java.util.Random;
import org.junit.jupiter.api.Test;

class MecanumDrivePoseEstimatorTest {
  @Test
  @SuppressWarnings("LocalVariableName")
  void testAccuracy() {
    var kinematics =
        new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var estimator =
        new MecanumDrivePoseEstimator(
            new Rotation2d(),
            new Pose2d(),
            kinematics,
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.05),
            VecBuilder.fill(0.1, 0.1, 0.1));

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(
                new Pose2d(),
                new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                new Pose2d(10, 10, Rotation2d.fromDegrees(180)),
                new Pose2d(30, 30, Rotation2d.fromDegrees(0)),
                new Pose2d(20, 20, Rotation2d.fromDegrees(180)),
                new Pose2d(10, 10, Rotation2d.fromDegrees(0))),
            new TrajectoryConfig(0.5, 2));

    var rand = new Random(5190);

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

      var wheelSpeeds =
          kinematics.toWheelSpeeds(
              new ChassisSpeeds(
                  groundTruthState.velocityMetersPerSecond,
                  0,
                  groundTruthState.velocityMetersPerSecond
                      * groundTruthState.curvatureRadPerMeter));

      wheelSpeeds.frontLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.frontRightMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearRightMetersPerSecond += rand.nextGaussian() * 0.1;

      var xHat =
          estimator.updateWithTime(
              t,
              groundTruthState
                  .poseMeters
                  .getRotation()
                  .plus(new Rotation2d(rand.nextGaussian() * 0.05)),
              wheelSpeeds);

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
    assertEquals(0.0, maxError, 0.1, "Incorrect max error");
  }
}
