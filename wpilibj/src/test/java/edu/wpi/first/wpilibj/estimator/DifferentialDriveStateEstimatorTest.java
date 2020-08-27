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

import org.ejml.dense.row.CommonOps_DDRM;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.ControlAffinePlantInversionFeedforward;
import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVoltageConstraint;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;


import static org.junit.jupiter.api.Assertions.assertEquals;

@SuppressWarnings({"PMD.AvoidInstantiatingObjectsInLoops",
      "PMD.ExcessiveMethodLength", "ParameterName", "LocalVariableName"})
public class DifferentialDriveStateEstimatorTest {
  private void scaleCappedU(Matrix<N2, N1> u) {
    boolean isOutputCapped = Math.abs(u.get(0, 0)) > 12.0 || Math.abs(u.get(1, 0)) > 12.0;

    if (isOutputCapped) {
      u.times(12.0 / CommonOps_DDRM.elementMaxAbs(u.getStorage().getDDRM()));
    }
  }

  @Test
  public void testAccuracy() {
    final double dt = 0.00505;

    final LinearSystem<N2, N2, N2> plant = LinearSystemId.identifyDrivetrainSystem(
        3.02, 0.642, 1.382, 0.08495);
    var kinematics = new DifferentialDriveKinematics(0.990405073902434);

    var estimator = new DifferentialDriveStateEstimator(
            plant,
            MatrixUtils.zeros(Nat.N10()),
            new MatBuilder<>(Nat.N10(), Nat.N1()).fill(
                0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0, 10.0, 2.0),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.005, 0.005),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.5, 0.5, 0.5),
            kinematics);

    var feedforward = new ControlAffinePlantInversionFeedforward<>(Nat.N10(), Nat.N2(),
        estimator::getDynamics, dt);

    var config = new TrajectoryConfig(12 / 2.5, (12 / 0.642) - 17.5);
    config.addConstraint(new DifferentialDriveKinematicsConstraint(kinematics, 12.0 / 2.5));
    config.addConstraint(new DifferentialDriveVoltageConstraint(
        new SimpleMotorFeedforward(0, 3.02, 0.642),
        kinematics,
        12));

    var traj = TrajectoryGenerator.generateTrajectory(
            new Pose2d(),
            List.of(),
            new Pose2d(4.8768, 2.7432, new Rotation2d()),
            config);

    var rand = new Random(604);

    List<Double> time = new ArrayList<>();
    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> trajLeftVel = new ArrayList<>();
    List<Double> trajRightVel = new ArrayList<>();
    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> observerLeftVelocity = new ArrayList<>();
    List<Double> observerRightVelocity = new ArrayList<>();
    List<Double> observerLeftVoltageError = new ArrayList<>();
    List<Double> observerRightVoltageError = new ArrayList<>();
    List<Double> observerAngularVelocityError = new ArrayList<>();
    List<Double> visionXs = new ArrayList<>();
    List<Double> visionYs = new ArrayList<>();

    double t = 0.0;

    final double visionUpdateRate = 0.1;
    Pose2d lastVisionPose = null;
    double lastVisionUpdateTime = Double.NEGATIVE_INFINITY;

    double distanceLeft = 0.0;
    double distanceRight = 0.0;

    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;

    Trajectory.State groundTruthState;

    Matrix<N2, N1> input;

    while (t <= traj.getTotalTimeSeconds()) {
      groundTruthState = traj.sample(t);

      var chassisSpeeds = new ChassisSpeeds(
          groundTruthState.velocityMetersPerSecond, 0,
          groundTruthState.velocityMetersPerSecond * groundTruthState.curvatureRadPerMeter
      );
      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      var x = new MatBuilder<>(Nat.N10(), Nat.N1()).fill(
              groundTruthState.poseMeters.getTranslation().getX(),
              groundTruthState.poseMeters.getTranslation().getY(),
              groundTruthState.poseMeters.getRotation().getRadians(),
              wheelSpeeds.leftMetersPerSecond,
              wheelSpeeds.rightMetersPerSecond,
              0.0, 0.0, 0.0, 0.0, 0.0);

      input = feedforward.calculate(x, feedforward.getR());

      scaleCappedU(input);

      if (lastVisionUpdateTime + visionUpdateRate + rand.nextGaussian() * 0.4 < t) {
        if (lastVisionPose != null) {
          estimator.applyPastGlobalMeasurement(
                  lastVisionPose,
                  lastVisionUpdateTime);
        }
        var groundPose = groundTruthState.poseMeters;
        lastVisionPose = new Pose2d(
                new Translation2d(
                        groundPose.getTranslation().getX() + rand.nextGaussian() * 0.5,
                        groundPose.getTranslation().getY() + rand.nextGaussian() * 0.5
                ),
                new Rotation2d(rand.nextGaussian() * 0.5).plus(groundPose.getRotation())
        );
        lastVisionUpdateTime = t;

        visionXs.add(lastVisionPose.getTranslation().getX());
        visionYs.add(lastVisionPose.getTranslation().getY());
      }

      distanceLeft += wheelSpeeds.leftMetersPerSecond * dt;
      distanceRight += wheelSpeeds.rightMetersPerSecond * dt;

      distanceLeft += rand.nextGaussian() * 0.001;
      distanceRight += rand.nextGaussian() * 0.001;

      var rotNoise = new Rotation2d(rand.nextGaussian() * 0.0001);

      var xHat = estimator.updateWithTime(groundTruthState.poseMeters.getRotation()
                      .plus(rotNoise).getRadians(),
                      distanceLeft,
                      distanceRight,
                      input,
                      t);

      double error =
              groundTruthState.poseMeters.getTranslation().getDistance(
                      new Translation2d(xHat.get(0, 0),
                                        xHat.get(1, 0)));
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      time.add(t);

      trajXs.add(groundTruthState.poseMeters.getTranslation().getX());
      trajYs.add(groundTruthState.poseMeters.getTranslation().getY());
      trajLeftVel.add(wheelSpeeds.leftMetersPerSecond);
      trajRightVel.add(wheelSpeeds.rightMetersPerSecond);

      observerXs.add(xHat.get(0, 0));
      observerYs.add(xHat.get(1, 0));
      observerLeftVelocity.add(xHat.get(3, 0));
      observerRightVelocity.add(xHat.get(4, 0));
      observerLeftVoltageError.add(xHat.get(7, 0));
      observerRightVoltageError.add(xHat.get(8, 0));
      observerAngularVelocityError.add(xHat.get(9, 0));

      t += dt;
    }

    assertEquals(
           0.0, errorSum / (traj.getTotalTimeSeconds() / dt), 0.5,
           "Incorrect mean error"
    );
    assertEquals(
           0.0, maxError, 0.8,
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

    var chartBuilderError = new XYChartBuilder();
    chartBuilderError.title = "Error versus Time";
    var chartError = chartBuilderError.build();

    chartError.addSeries("LeftVoltage", time, observerLeftVoltageError);
    chartError.addSeries("RightVoltage", time, observerRightVoltageError);
    chartError.addSeries("AngularVelocity", time, observerAngularVelocityError);
    charts.add(chartError);

    var chartBuilderLeftVelocity = new XYChartBuilder();
    chartBuilderLeftVelocity.title = "Left Velocity versus Time";
    var chartLeftVelocity = chartBuilderLeftVelocity.build();

    chartLeftVelocity.addSeries("xHat", time, observerLeftVelocity);
    chartLeftVelocity.addSeries("Trajectory", time, trajLeftVel);
    charts.add(chartLeftVelocity);

    var chartBuilderRightVelocity = new XYChartBuilder();
    chartBuilderRightVelocity.title = "Right Velocity versus Time";
    var chartRightVelocity = chartBuilderRightVelocity.build();

    chartRightVelocity.addSeries("xHat", time, observerRightVelocity);
    chartRightVelocity.addSeries("Trajectory", time, trajRightVel);

    charts.add(chartRightVelocity);

    new SwingWrapper<>(charts).displayChartMatrix();
    try {
      Thread.sleep(1000000000);
    } catch (InterruptedException e) {
    }
    */
  }
}
