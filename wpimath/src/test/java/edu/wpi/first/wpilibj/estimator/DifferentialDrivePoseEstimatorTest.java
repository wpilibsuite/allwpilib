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
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveWheelSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Nat;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class DifferentialDrivePoseEstimatorTest {
  @SuppressWarnings({"LocalVariableName", "PMD.ExcessiveMethodLength",
        "PMD.AvoidInstantiatingObjectsInLoops"})
  @Test
  public void testAccuracy() {
    var estimator = new DifferentialDrivePoseEstimator(new Rotation2d(), new Pose2d(),
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.02, 0.02, 0.01, 0.02, 0.02),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.01, 0.01, 0.001),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.1, 0.1, 0.01));

    var traj = TrajectoryGenerator.generateTrajectory(
            List.of(
                    new Pose2d(0, 0, Rotation2d.fromDegrees(45)),
                    new Pose2d(3, 0, Rotation2d.fromDegrees(-90)),
                    new Pose2d(0, 0, Rotation2d.fromDegrees(135)),
                    new Pose2d(-3, 0, Rotation2d.fromDegrees(-90)),
                    new Pose2d(0, 0, Rotation2d.fromDegrees(45))
            ),
            new TrajectoryConfig(10, 5));


    var kinematics = new DifferentialDriveKinematics(1);
    var rand = new Random(4915);

    final double dt = 0.02;
    double t = 0.0;

    final double visionUpdateRate = 0.1;
    Pose2d lastVisionPose = null;
    double lastVisionUpdateTime = Double.NEGATIVE_INFINITY;

    double distanceLeft = 0.0;
    double distanceRight = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    Trajectory.State groundtruthState;
    DifferentialDriveWheelSpeeds input;
    while (t <= traj.getTotalTimeSeconds()) {
      groundtruthState = traj.sample(t);
      input = kinematics.toWheelSpeeds(new ChassisSpeeds(
              groundtruthState.velocityMetersPerSecond, 0.0,
              // ds/dt * dtheta/ds = dtheta/dt
              groundtruthState.velocityMetersPerSecond * groundtruthState.curvatureRadPerMeter
      ));

      if (lastVisionUpdateTime + visionUpdateRate + rand.nextGaussian() * 0.4 < t) {
        if (lastVisionPose != null) {
          estimator.addVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
        }
        var groundPose = groundtruthState.poseMeters;
        lastVisionPose = new Pose2d(
                new Translation2d(
                        groundPose.getTranslation().getX() + rand.nextGaussian() * 0.1,
                        groundPose.getTranslation().getY() + rand.nextGaussian() * 0.1
                ),
                new Rotation2d(rand.nextGaussian() * 0.01).plus(groundPose.getRotation())
        );
        lastVisionUpdateTime = t;
      }

      input.leftMetersPerSecond += rand.nextGaussian() * 0.01;
      input.rightMetersPerSecond += rand.nextGaussian() * 0.01;

      distanceLeft += input.leftMetersPerSecond * dt;
      distanceRight += input.rightMetersPerSecond * dt;

      var rotNoise = new Rotation2d(rand.nextGaussian() * 0.001);
      var xHat = estimator.updateWithTime(
              t,
              groundtruthState.poseMeters.getRotation().plus(rotNoise),
              input,
              distanceLeft, distanceRight
      );

      double error =
              groundtruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      t += dt;
    }

    assertEquals(
            0.0, errorSum / (traj.getTotalTimeSeconds() / dt), 0.035,
            "Incorrect mean error"
    );
    assertEquals(
            0.0, maxError, 0.055,
            "Incorrect max error"
    );
  }
}
