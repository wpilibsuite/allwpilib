/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.List;
import java.util.Random;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.SwerveDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.VecBuilder;


import static org.junit.jupiter.api.Assertions.assertEquals;

public class SwerveDrivePoseEstimatorTest {
  @Test
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops",
        "PMD.ExcessiveMethodLength"})
  public void testAccuracy() {
    var kinematics = new SwerveDriveKinematics(
            new Translation2d(1, 1),
            new Translation2d(1, -1),
            new Translation2d(-1, -1),
            new Translation2d(-1, 1)
    );
    var estimator = new SwerveDrivePoseEstimator(
            new Rotation2d(), new Pose2d(), kinematics,
            VecBuilder.fill(0.1, 0.1, 0.1),
            VecBuilder.fill(0.005),
            VecBuilder.fill(0.1, 0.1, 0.1)
    );

    var trajectory = TrajectoryGenerator.generateTrajectory(
            List.of(
                    new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                    new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                    new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                    new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                    new Pose2d(0, 0, Rotation2d.fromDegrees(45))
            ),
            new TrajectoryConfig(0.5, 2)
    );

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

        lastVisionPose = new Pose2d(
                new Translation2d(
                        groundTruthState.poseMeters.getTranslation().getX()
                                + rand.nextGaussian() * 0.1,
                        groundTruthState.poseMeters.getTranslation().getY()
                                + rand.nextGaussian() * 0.1
                ),
                new Rotation2d(
                        rand.nextGaussian() * 0.1).plus(groundTruthState.poseMeters.getRotation())
        );

        lastVisionUpdateTime = t;

      }

      var moduleStates = kinematics.toSwerveModuleStates(new ChassisSpeeds(
              groundTruthState.velocityMetersPerSecond,
              0.0,
              groundTruthState.velocityMetersPerSecond * groundTruthState.curvatureRadPerMeter)
      );
      for (var moduleState : moduleStates) {
        moduleState.angle = moduleState.angle.plus(new Rotation2d(rand.nextGaussian() * 0.005));
        moduleState.speedMetersPerSecond += rand.nextGaussian() * 0.1;
      }

      var xHat = estimator.updateWithTime(
              t,
              groundTruthState.poseMeters.getRotation()
                      .plus(new Rotation2d(rand.nextGaussian() * 0.05)),
              moduleStates);

      double error =
              groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
            0.0, errorSum / (trajectory.getTotalTimeSeconds() / dt), 0.25,
            "Incorrect mean error"
    );
    assertEquals(
            0.0, maxError, 0.42,
            "Incorrect max error"
    );
  }
}
