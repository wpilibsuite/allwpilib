/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.MecanumDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.VecBuilder;


import static org.junit.jupiter.api.Assertions.assertEquals;

public class MecanumDrivePoseEstimatorTest {
  @Test
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops",
        "PMD.ExcessiveMethodLength"})
  public void testAccuracy() {
    var kinematics = new MecanumDriveKinematics(
            new Translation2d(1, 1), new Translation2d(1, -1),
            new Translation2d(-1, -1), new Translation2d(-1, 1));

    var estimator = new MecanumDrivePoseEstimator(
            new Rotation2d(), new Pose2d(), kinematics,
            VecBuilder.fill(0.01, 0.01, 0.01),
            VecBuilder.fill(0.05),
            VecBuilder.fill(0.1, 0.1, 0.1)
    );

    var trajectory = TrajectoryGenerator.generateTrajectory(
            List.of(new Pose2d(),
                    new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                    new Pose2d(10, 10, Rotation2d.fromDegrees(180)),
                    new Pose2d(30, 30, Rotation2d.fromDegrees(0)),
                    new Pose2d(20, 20, Rotation2d.fromDegrees(180)),
                    new Pose2d(10, 10, Rotation2d.fromDegrees(0))),
            new TrajectoryConfig(0.5, 2)
    );

    var rand = new Random(5190);

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> trajCosine = new ArrayList<>();
    List<Double> trajSine = new ArrayList<>();
    List<Double> trajTheta = new ArrayList<>();

    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> observerCosine = new ArrayList<>();
    List<Double> observerSine = new ArrayList<>();
    List<Double> observerTheta = new ArrayList<>();

    List<Double> visionXs = new ArrayList<>();
    List<Double> visionYs = new ArrayList<>();
    List<Double> visionCosine = new ArrayList<>();
    List<Double> visionSine = new ArrayList<>();
    List<Double> visionTheta = new ArrayList<>();

    List<Double> time = new ArrayList<>();
    List<Double> visionTime = new ArrayList<>();

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

        visionXs.add(lastVisionPose.getTranslation().getX());
        visionYs.add(lastVisionPose.getTranslation().getY());
        visionTheta.add(lastVisionPose.getRotation().getDegrees());
        visionCosine.add(lastVisionPose.getRotation().getCos());
        visionSine.add(lastVisionPose.getRotation().getSin());
        visionTime.add(t);
      }

      var wheelSpeeds = kinematics.toWheelSpeeds(new ChassisSpeeds(
              groundTruthState.velocityMetersPerSecond, 0,
              groundTruthState.velocityMetersPerSecond * groundTruthState.curvatureRadPerMeter));

      wheelSpeeds.frontLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.frontRightMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearLeftMetersPerSecond += rand.nextGaussian() * 0.1;
      wheelSpeeds.rearRightMetersPerSecond += rand.nextGaussian() * 0.1;

      var xHat = estimator.updateWithTime(t,
              groundTruthState.poseMeters.getRotation()
                      .plus(new Rotation2d(rand.nextGaussian() * 0.05)), wheelSpeeds);

      double error =
              groundTruthState.poseMeters.getTranslation().getDistance(xHat.getTranslation());
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      trajXs.add(groundTruthState.poseMeters.getTranslation().getX());
      trajYs.add(groundTruthState.poseMeters.getTranslation().getY());
      trajTheta.add(groundTruthState.poseMeters.getRotation().getDegrees());
      trajCosine.add(groundTruthState.poseMeters.getRotation().getCos());
      trajSine.add(groundTruthState.poseMeters.getRotation().getSin());
      observerXs.add(xHat.getTranslation().getX());
      observerYs.add(xHat.getTranslation().getY());
      observerTheta.add(xHat.getRotation().getDegrees());
      observerCosine.add(xHat.getRotation().getCos());
      observerSine.add(xHat.getRotation().getSin());
      time.add(t);

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

    /*
    List<XYChart> charts = new ArrayList<XYChart>();

    var chartBuilder = new XYChartBuilder();
    chartBuilder.title = "The Magic of Sensor Fusion";
    var chart = chartBuilder.build();

    chart.addSeries("Vision", visionXs, visionYs);
    chart.addSeries("Trajectory", trajXs, trajYs);
    chart.addSeries("xHat", observerXs, observerYs);
    charts.add(chart);

    var chartBuilder1 = new XYChartBuilder();
    chartBuilder1.title = "Cosine";
    var chart1 = chartBuilder1.build();

    chart1.addSeries("Vision", visionTime, visionCosine);
    chart1.addSeries("Trajectory", time, trajCosine);
    chart1.addSeries("xHat", time, observerCosine);
    charts.add(chart1);

    var chartBuilder2 = new XYChartBuilder();
    chartBuilder2.title = "Sine";
    var chart2 = chartBuilder2.build();

    chart2.addSeries("Vision", visionTime, visionSine);
    chart2.addSeries("Trajectory", time, trajSine);
    chart2.addSeries("xHat", time, observerSine);

    charts.add(chart2);

    var chartBuilder3 = new XYChartBuilder();
    chartBuilder3.title = "Degrees";
    var chart3 = chartBuilder3.build();

    chart3.addSeries("Vision", visionTime, visionTheta);
    chart3.addSeries("Trajectory", time, trajTheta);
    chart3.addSeries("xHat", time, observerTheta);
    charts.add(chart3);



    new SwingWrapper<>(charts).displayChartMatrix();
    try {
      Thread.sleep(1000000000);
    } catch (InterruptedException e) {
    }
    */
  }
}
