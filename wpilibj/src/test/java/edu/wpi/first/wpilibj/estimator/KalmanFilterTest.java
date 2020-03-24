package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.LinearQuadraticRegulatorTest;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N6;

import static edu.wpi.first.wpilibj.controller.LinearSystemLoopTest.kDt;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class KalmanFilterTest {

  static {
    LinearQuadraticRegulatorTest.createArm();
    LinearQuadraticRegulatorTest.createElevator();
  }

  // A swerve drive system where the states are [x, y, theta, vx, vy, vTheta]^T,
  // Y is [x, y, theta]^T and u is [ax, ay, alpha}^T
  LinearSystem<N6, N3, N3> m_swerveObserverSystem = new LinearSystem<>(Nat.N6(), Nat.N3(),
          Nat.N3(),
          new MatBuilder<>(Nat.N6(), Nat.N6()).fill( // A
                  0, 0, 0, 1, 0, 0,
                  0, 0, 0, 0, 1, 0,
                  0, 0, 0, 0, 0, 1,
                  0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0),
          new MatBuilder<>(Nat.N6(), Nat.N3()).fill( // B
                  0, 0, 0,
                  0, 0, 0,
                  0, 0, 0,
                  1, 0, 0,
                  0, 1, 0,
                  0, 0, 1
          ),
          new MatBuilder<>(Nat.N3(), Nat.N6()).fill( // C
                  1, 0, 0, 0, 0, 0,
                  0, 1, 0, 0, 0, 0,
                  0, 0, 1, 0, 0, 0
          ),
          new Matrix<>(new SimpleMatrix(3, 3)), // D
          new MatBuilder<>(Nat.N3(), Nat.N1()).fill(-4, -4, -12), // uMin,
          new MatBuilder<>(Nat.N3(), Nat.N1()).fill(4, 4, 12));

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testElevatorKalmanFilter() {
    var plant = LinearQuadraticRegulatorTest.elevatorPlant;

    var Q = new MatBuilder<>(Nat.N2(), Nat.N1()).fill(0.05, 1.0);
    var R = new MatBuilder<>(Nat.N1(), Nat.N1()).fill(0.0001);

    var filter = new KalmanFilter<>(Nat.N2(), Nat.N1(), plant, Q, R, kDt);

    var p = filter.getP();
    var gain = filter.getXhat();

    System.out.printf("p: \n%s\n: gain: \n%s\n", p, gain);
  }

  @Test
  public void testSwerveKFStationary() {

    var random = new Random();
    m_swerveObserverSystem.reset();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
            m_swerveObserverSystem,
            new MatBuilder<>(Nat.N6(), Nat.N1()).fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
            // weights
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(2, 2, 2), // measurement weights
            0.020
    );

    List<Double> xhatsX = new ArrayList<>();
    List<Double> measurementsX = new ArrayList<>();
    List<Double> xhatsY = new ArrayList<>();
    List<Double> measurementsY = new ArrayList<>();

    for (int i = 0; i < 100; i++) {
      // the robot is at [0, 0, 0] so we just park here
      var measurement = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
              random.nextGaussian(), random.nextGaussian(), random.nextGaussian());
      filter.correct(new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0), measurement);

      // we continue to not accelerate
      filter.predict(new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0), 0.020);

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
    assertEquals(0.0, m_swerveObserverSystem.getX(0), 0.3);
    assertEquals(0.0, m_swerveObserverSystem.getX(0), 0.3);
  }

  @Test
  public void testSwerveKFMovingWithoutAccelerating() {

    var random = new Random();
    m_swerveObserverSystem.reset();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
            m_swerveObserverSystem,
            new MatBuilder<>(Nat.N6(), Nat.N1()).fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
            // weights
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(4, 4, 4), // measurement weights
            0.020
    );

    List<Double> xhatsX = new ArrayList<>();
    List<Double> measurementsX = new ArrayList<>();
    List<Double> xhatsY = new ArrayList<>();
    List<Double> measurementsY = new ArrayList<>();

    // we set the velocity of the robot so that it's moving forward slowly
    m_swerveObserverSystem.setX(0, 0.5);
    m_swerveObserverSystem.setX(1, 0.5);

    for (int i = 0; i < 300; i++) {
      // the robot is at [0, 0, 0] so we just park here
      var measurement = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
              random.nextGaussian() / 10d,
              random.nextGaussian() / 10d,
              random.nextGaussian() / 4d // std dev of [1, 1, 1]
      );

      filter.correct(new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0), measurement);

      // we continue to not accelerate
      filter.predict(new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0), 0.020);

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

    assertEquals(0.0, m_swerveObserverSystem.getX(0), 0.2);
    assertEquals(0.0, m_swerveObserverSystem.getX(1), 0.2);
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testSwerveKFMovingOverTrajectory() {

    var random = new Random();
    m_swerveObserverSystem.reset();

    var filter = new KalmanFilter<>(Nat.N6(), Nat.N3(),
            m_swerveObserverSystem,
            new MatBuilder<>(Nat.N6(), Nat.N1()).fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1), // state
            // weights
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(4, 4, 4), // measurement weights
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
    var lastVelocity = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0, 0.0, 0.0);

    while (time <= trajectory.getTotalTimeSeconds()) {
      var sample = trajectory.sample(time);
      var measurement = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
              sample.poseMeters.getTranslation().getX() + random.nextGaussian() / 5d,
              sample.poseMeters.getTranslation().getY() + random.nextGaussian() / 5d,
              sample.poseMeters.getRotation().getRadians() + random.nextGaussian() / 3d
      );

      var velocity = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
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
            .getTranslation().getX(), m_swerveObserverSystem.getX(0), 0.2);
    assertEquals(trajectory.sample(trajectory.getTotalTimeSeconds()).poseMeters
            .getTranslation().getY(), m_swerveObserverSystem.getX(1), 0.2);
  }
}
