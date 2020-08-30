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
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N6;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class KalmanFilterTest {
  private static LinearSystem<N2, N1, N1> elevatorPlant;

  private static final double kDt = 0.00505;

  static {
    createElevator();
  }

  @SuppressWarnings("LocalVariableName")
  public static void createElevator() {
    var motors = DCMotor.getVex775Pro(2);

    var m = 5.0;
    var r = 0.0181864;
    var G = 1.0;
    elevatorPlant = LinearSystemId.createElevatorSystem(motors, m, r, G);
  }

  // A swerve drive system where the states are [x, y, theta, vx, vy, vTheta]^T,
  // Y is [x, y, theta]^T and u is [ax, ay, alpha}^T
  LinearSystem<N6, N3, N3> m_swerveObserverSystem = new LinearSystem<>(
      Matrix.mat(Nat.N6(), Nat.N6()).fill( // A
              0, 0, 0, 1, 0, 0,
              0, 0, 0, 0, 1, 0,
              0, 0, 0, 0, 0, 1,
              0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0),
        Matrix.mat(Nat.N6(), Nat.N3()).fill( // B
              0, 0, 0,
              0, 0, 0,
              0, 0, 0,
              1, 0, 0,
              0, 1, 0,
              0, 0, 1
        ),
        Matrix.mat(Nat.N3(), Nat.N6()).fill( // C
              1, 0, 0, 0, 0, 0,
              0, 1, 0, 0, 0, 0,
              0, 0, 1, 0, 0, 0
        ),
        new Matrix<>(Nat.N3(), Nat.N3())); // D

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testElevatorKalmanFilter() {

    var Q = VecBuilder.fill(0.05, 1.0);
    var R = VecBuilder.fill(0.0001);

    assertDoesNotThrow(() -> new KalmanFilter<>(Nat.N2(), Nat.N1(), elevatorPlant, Q, R, kDt));
  }

  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  public void testSwerveKFStationary() {

    var random = new Random();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
          m_swerveObserverSystem,
          VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
          // weights
          VecBuilder.fill(2, 2, 2), // measurement weights
          0.020
    );

    List<Double> xhatsX = new ArrayList<>();
    List<Double> measurementsX = new ArrayList<>();
    List<Double> xhatsY = new ArrayList<>();
    List<Double> measurementsY = new ArrayList<>();

    Matrix<N3, N1> measurement;
    for (int i = 0; i < 100; i++) {
      // the robot is at [0, 0, 0] so we just park here
      measurement = VecBuilder.fill(
            random.nextGaussian(), random.nextGaussian(), random.nextGaussian());
      filter.correct(VecBuilder.fill(0.0, 0.0, 0.0), measurement);

      // we continue to not accelerate
      filter.predict(VecBuilder.fill(0.0, 0.0, 0.0), 0.020);

      measurementsX.add(measurement.get(0, 0));
      measurementsY.add(measurement.get(1, 0));
      xhatsX.add(filter.getXhat(0));
      xhatsY.add(filter.getXhat(1));
    }

    //var chart = new XYChartBuilder().build();
    //chart.addSeries("Xhat, x/y", xhatsX, xhatsY);
    //chart.addSeries("Measured position, x/y", measurementsX, measurementsY);
    //try {
    //  new SwingWrapper<>(chart).displayChart();
    //  Thread.sleep(10000000);
    //} catch (Exception ign) {
    //}
    assertEquals(0.0, filter.getXhat(0), 0.3);
    assertEquals(0.0, filter.getXhat(0), 0.3);
  }

  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  public void testSwerveKFMovingWithoutAccelerating() {

    var random = new Random();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
          m_swerveObserverSystem,
          VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
          // weights
          VecBuilder.fill(4, 4, 4), // measurement weights
          0.020
    );

    List<Double> xhatsX = new ArrayList<>();
    List<Double> measurementsX = new ArrayList<>();
    List<Double> xhatsY = new ArrayList<>();
    List<Double> measurementsY = new ArrayList<>();

    // we set the velocity of the robot so that it's moving forward slowly
    filter.setXhat(0, 0.5);
    filter.setXhat(1, 0.5);

    for (int i = 0; i < 300; i++) {
      // the robot is at [0, 0, 0] so we just park here
      var measurement = VecBuilder.fill(
            random.nextGaussian() / 10d,
            random.nextGaussian() / 10d,
            random.nextGaussian() / 4d // std dev of [1, 1, 1]
      );

      filter.correct(VecBuilder.fill(0.0, 0.0, 0.0), measurement);

      // we continue to not accelerate
      filter.predict(VecBuilder.fill(0.0, 0.0, 0.0), 0.020);

      measurementsX.add(measurement.get(0, 0));
      measurementsY.add(measurement.get(1, 0));
      xhatsX.add(filter.getXhat(0));
      xhatsY.add(filter.getXhat(1));
    }

    //var chart = new XYChartBuilder().build();
    //chart.addSeries("Xhat, x/y", xhatsX, xhatsY);
    //chart.addSeries("Measured position, x/y", measurementsX, measurementsY);
    //try {
    //  new SwingWrapper<>(chart).displayChart();
    //  Thread.sleep(10000000);
    //} catch (Exception ign) {}

    assertEquals(0.0, filter.getXhat(0), 0.2);
    assertEquals(0.0, filter.getXhat(1), 0.2);
  }

  @Test
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public void testSwerveKFMovingOverTrajectory() {

    var random = new Random();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
          m_swerveObserverSystem,
          VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
          // weights
          VecBuilder.fill(4, 4, 4), // measurement weights
          0.020
    );

    List<Double> xhatsX = new ArrayList<>();
    List<Double> measurementsX = new ArrayList<>();
    List<Double> xhatsY = new ArrayList<>();
    List<Double> measurementsY = new ArrayList<>();

    var trajectory = TrajectoryGenerator.generateTrajectory(
          List.of(
                new Pose2d(0, 0, new Rotation2d()),
                new Pose2d(5, 5, new Rotation2d())
          ), new TrajectoryConfig(2, 2)
    );
    var time = 0.0;
    var lastVelocity = VecBuilder.fill(0.0, 0.0, 0.0);

    while (time <= trajectory.getTotalTimeSeconds()) {
      var sample = trajectory.sample(time);
      var measurement = VecBuilder.fill(
            sample.poseMeters.getTranslation().getX() + random.nextGaussian() / 5d,
            sample.poseMeters.getTranslation().getY() + random.nextGaussian() / 5d,
            sample.poseMeters.getRotation().getRadians() + random.nextGaussian() / 3d
      );

      var velocity = VecBuilder.fill(
            sample.velocityMetersPerSecond * sample.poseMeters.getRotation().getCos(),
            sample.velocityMetersPerSecond * sample.poseMeters.getRotation().getSin(),
            sample.curvatureRadPerMeter * sample.velocityMetersPerSecond
      );
      var u = (velocity.minus(lastVelocity)).div(0.020);
      lastVelocity = velocity;

      filter.correct(u, measurement);
      filter.predict(u, 0.020);

      measurementsX.add(measurement.get(0, 0));
      measurementsY.add(measurement.get(1, 0));
      xhatsX.add(filter.getXhat(0));
      xhatsY.add(filter.getXhat(1));

      time += 0.020;
    }

    //var chart = new XYChartBuilder().build();
    //chart.addSeries("Xhat, x/y", xhatsX, xhatsY);
    //chart.addSeries("Measured position, x/y", measurementsX, measurementsY);
    //try {
    //        new SwingWrapper<>(chart).displayChart();
    //        Thread.sleep(10000000);
    //} catch (Exception ign) {
    //}

    assertEquals(trajectory.sample(trajectory.getTotalTimeSeconds()).poseMeters
          .getTranslation().getX(), filter.getXhat(0), 0.2);
    assertEquals(trajectory.sample(trajectory.getTotalTimeSeconds()).poseMeters
          .getTranslation().getY(), filter.getXhat(1), 0.2);
  }
}
