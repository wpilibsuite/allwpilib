// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N3;
import edu.wpi.first.math.numbers.N5;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.NumericalJacobian;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import org.junit.jupiter.api.Test;

class ExtendedKalmanFilterTest {
  private static Matrix<N5, N1> getDynamics(final Matrix<N5, N1> x, final Matrix<N2, N1> u) {
    final var motors = DCMotor.getCIM(2);

    final var gr = 7.08; // Gear ratio
    final var rb = 0.8382 / 2.0; // Wheelbase radius (track width)
    final var r = 0.0746125; // Wheel radius
    final var m = 63.503; // Robot mass
    final var J = 5.6; // Robot moment of inertia

    final var C1 =
        -Math.pow(gr, 2) * motors.KtNMPerAmp / (motors.KvRadPerSecPerVolt * motors.rOhms * r * r);
    final var C2 = gr * motors.KtNMPerAmp / (motors.rOhms * r);
    final var k1 = 1.0 / m + rb * rb / J;
    final var k2 = 1.0 / m - rb * rb / J;

    final var vl = x.get(3, 0);
    final var vr = x.get(4, 0);
    final var Vl = u.get(0, 0);
    final var Vr = u.get(1, 0);

    final Matrix<N5, N1> result = new Matrix<>(Nat.N5(), Nat.N1());
    final var v = 0.5 * (vl + vr);
    result.set(0, 0, v * Math.cos(x.get(2, 0)));
    result.set(1, 0, v * Math.sin(x.get(2, 0)));
    result.set(2, 0, (vr - vl) / (2.0 * rb));
    result.set(3, 0, k1 * ((C1 * vl) + (C2 * Vl)) + k2 * ((C1 * vr) + (C2 * Vr)));
    result.set(4, 0, k2 * ((C1 * vl) + (C2 * Vl)) + k1 * ((C1 * vr) + (C2 * Vr)));
    return result;
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @Test
  void testInit() {
    double dtSeconds = 0.00505;

    assertDoesNotThrow(
        () -> {
          ExtendedKalmanFilter<N5, N2, N3> observer =
              new ExtendedKalmanFilter<>(
                  Nat.N5(),
                  Nat.N2(),
                  Nat.N3(),
                  ExtendedKalmanFilterTest::getDynamics,
                  ExtendedKalmanFilterTest::getLocalMeasurementModel,
                  VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                  VecBuilder.fill(0.0001, 0.01, 0.01),
                  dtSeconds);

          Matrix<N2, N1> u = VecBuilder.fill(12.0, 12.0);
          observer.predict(u, dtSeconds);

          var localY = getLocalMeasurementModel(observer.getXhat(), u);
          observer.correct(u, localY);

          var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
          var R = StateSpaceUtil.makeCostMatrix(VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
          observer.correct(
              Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
        });
  }

  @Test
  void testConvergence() {
    double dtSeconds = 0.00505;
    double rbMeters = 0.8382 / 2.0; // Robot radius

    ExtendedKalmanFilter<N5, N2, N3> observer =
        new ExtendedKalmanFilter<>(
            Nat.N5(),
            Nat.N2(),
            Nat.N3(),
            ExtendedKalmanFilterTest::getDynamics,
            ExtendedKalmanFilterTest::getLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
            VecBuilder.fill(0.001, 0.01, 0.01),
            dtSeconds);

    List<Pose2d> waypoints =
        List.of(
            new Pose2d(2.75, 22.521, Rotation2d.kZero),
            new Pose2d(24.73, 19.68, Rotation2d.fromDegrees(5.846)));
    var trajectory =
        TrajectoryGenerator.generateTrajectory(waypoints, new TrajectoryConfig(8.8, 0.1));

    Matrix<N5, N1> r = new Matrix<>(Nat.N5(), Nat.N1());

    Matrix<N5, N1> nextR = new Matrix<>(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = new Matrix<>(Nat.N2(), Nat.N1());

    var B =
        NumericalJacobian.numericalJacobianU(
            Nat.N5(),
            Nat.N2(),
            ExtendedKalmanFilterTest::getDynamics,
            new Matrix<>(Nat.N5(), Nat.N1()),
            u);

    observer.setXhat(
        VecBuilder.fill(
            trajectory.getInitialPose().getTranslation().getX(),
            trajectory.getInitialPose().getTranslation().getY(),
            trajectory.getInitialPose().getRotation().getRadians(),
            0.0,
            0.0));

    var groundTruthX = observer.getXhat();

    double totalTime = trajectory.getTotalTimeSeconds();
    for (int i = 0; i < (totalTime / dtSeconds); i++) {
      var ref = trajectory.sample(dtSeconds * i);
      double vl = ref.velocityMetersPerSecond * (1 - (ref.curvatureRadPerMeter * rbMeters));
      double vr = ref.velocityMetersPerSecond * (1 + (ref.curvatureRadPerMeter * rbMeters));

      nextR.set(0, 0, ref.poseMeters.getTranslation().getX());
      nextR.set(1, 0, ref.poseMeters.getTranslation().getY());
      nextR.set(2, 0, ref.poseMeters.getRotation().getRadians());
      nextR.set(3, 0, vl);
      nextR.set(4, 0, vr);

      var localY = getLocalMeasurementModel(groundTruthX, u);
      var whiteNoiseStdDevs = VecBuilder.fill(0.0001, 0.5, 0.5);
      observer.correct(u, localY.plus(StateSpaceUtil.makeWhiteNoiseVector(whiteNoiseStdDevs)));

      Matrix<N5, N1> rdot = nextR.minus(r).div(dtSeconds);
      u = new Matrix<>(B.solve(rdot.minus(getDynamics(r, new Matrix<>(Nat.N2(), Nat.N1())))));

      observer.predict(u, dtSeconds);

      groundTruthX =
          NumericalIntegration.rk4(
              ExtendedKalmanFilterTest::getDynamics, groundTruthX, u, dtSeconds);

      r = nextR;
    }

    var localY = getLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
    var R = StateSpaceUtil.makeCostMatrix(VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);

    var finalPosition = trajectory.sample(trajectory.getTotalTimeSeconds());
    assertEquals(finalPosition.poseMeters.getTranslation().getX(), observer.getXhat(0), 1.0);
    assertEquals(finalPosition.poseMeters.getTranslation().getY(), observer.getXhat(1), 1.0);
    assertEquals(finalPosition.poseMeters.getRotation().getRadians(), observer.getXhat(2), 1.0);
    assertEquals(0.0, observer.getXhat(3), 1.0);
    assertEquals(0.0, observer.getXhat(4), 1.0);
  }
}
