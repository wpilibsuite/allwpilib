/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.stream.Collectors;

import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N6;
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
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.02, 0.02, 0.01),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.1, 0.1, 0.01));

    var traj = TrajectoryGenerator.generateTrajectory(
            List.of(
                    new Pose2d(),
                    new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                    new Pose2d(23, 23, Rotation2d.fromDegrees(173)),
                    new Pose2d(54, 54, new Rotation2d())
            ),
            new TrajectoryConfig(0.5, 2));

    var kinematics = new DifferentialDriveKinematics(1);
    var rand = new Random(4915);

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> visionXs = new ArrayList<>();
    List<Double> visionYs = new ArrayList<>();

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

        visionXs.add(lastVisionPose.getTranslation().getX());
        visionYs.add(lastVisionPose.getTranslation().getY());
      }

      input.leftMetersPerSecond += rand.nextGaussian() * 0.02;
      input.rightMetersPerSecond += rand.nextGaussian() * 0.02;

      distanceLeft += input.leftMetersPerSecond * dt;
      distanceRight += input.rightMetersPerSecond * dt;

      var rotNoise = new Rotation2d(rand.nextGaussian() * 0.01);
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

      trajXs.add(groundtruthState.poseMeters.getTranslation().getX());
      trajYs.add(groundtruthState.poseMeters.getTranslation().getY());
      observerXs.add(xHat.getTranslation().getX());
      observerYs.add(xHat.getTranslation().getY());

      t += dt;
    }

    assertEquals(
            0.0, errorSum / (traj.getTotalTimeSeconds() / dt), 0.03,
            "Incorrect mean error"
    );
    assertEquals(
            0.0, maxError, 0.05,
            "Incorrect max error"
    );

    //System.out.println("Mean error (meters): " + errorSum / (traj.getTotalTimeSeconds() / dt));
    //System.out.println("Max error (meters):  " + maxError);

    //var chartBuilder = new XYChartBuilder();
    //chartBuilder.title = "The Magic of Sensor Fusion";
    //var chart = chartBuilder.build();

    //chart.addSeries("Vision", visionXs, visionYs);
    //chart.addSeries("Trajectory", trajXs, trajYs);
    //chart.addSeries("xHat", observerXs, observerYs);

    //new SwingWrapper<>(chart).displayChart();
    //try {
    //  Thread.sleep(1000000000);
    //} catch (InterruptedException e) {
    //}
  }

  @Test void test() {
    var estimator = new DifferentialDrivePoseEstimator(new Rotation2d(), new Pose2d(),
        new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.02, 0.02, 0.01, 0.02, 0.02),
        new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.02, 0.02, 0.01),
        new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.1, 0.1, 0.01));

    System.out.println("x, y, cos(theta), sin(theta), dist_l, dist_r");

    estimator.m_observer.setXhat(VecBuilder.fill(0, 0, Math.cos(0), Math.sin(0), 0, 0));
    for(int i = 0; i < 50; i++) {
      estimator.m_observer.predict(VecBuilder.fill(1, 1, 0), 0.020);
    }

    var x = estimator.m_observer.getXhat();
    assertEquals(1, x.get(0, 0), 0.01);
    assertEquals(0, x.get(1, 0), 0.01);
    assertEquals(0, estimator.getEstimatedPosition().getRotation().getRadians(), 0.01);
  }

  //  String matToCSV(Matrix<?, ?> mat) {
  //    return Arrays.stream(mat.getStorage().getDDRM().getData())
  //        .mapToObj(String::valueOf).collect(Collectors.joining(","));
  //  }
}
