// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MatBuilder;
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
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.NumericalJacobian;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.List;
import org.junit.jupiter.api.Test;

class UnscentedKalmanFilterTest {
  private static Matrix<N5, N1> getDynamics(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    var motors = DCMotor.getCIM(2);

    // var gLow = 15.32;    // Low gear ratio
    var gHigh = 7.08; // High gear ratio
    var rb = 0.8382 / 2.0; // Robot radius
    var r = 0.0746125; // Wheel radius
    var m = 63.503; // Robot mass
    var J = 5.6; // Robot moment of inertia

    var C1 =
        -Math.pow(gHigh, 2)
            * motors.KtNMPerAmp
            / (motors.KvRadPerSecPerVolt * motors.rOhms * r * r);
    var C2 = gHigh * motors.KtNMPerAmp / (motors.rOhms * r);
    var k1 = 1.0 / m + Math.pow(rb, 2) / J;
    var k2 = 1.0 / m - Math.pow(rb, 2) / J;

    var vl = x.get(3, 0);
    var vr = x.get(4, 0);
    var Vl = u.get(0, 0);
    var Vr = u.get(1, 0);

    var v = 0.5 * (vl + vr);
    return VecBuilder.fill(
        v * Math.cos(x.get(2, 0)),
        v * Math.sin(x.get(2, 0)),
        (vr - vl) / (2.0 * rb),
        k1 * (C1 * vl + C2 * Vl) + k2 * (C1 * vr + C2 * Vr),
        k2 * (C1 * vl + C2 * Vl) + k1 * (C1 * vr + C2 * Vr));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return x.copy();
  }

  @Test
  void testInit() {
    var dtSeconds = 0.005;
    assertDoesNotThrow(
        () -> {
          UnscentedKalmanFilter<N5, N2, N3> observer =
              new UnscentedKalmanFilter<>(
                  Nat.N5(),
                  Nat.N3(),
                  UnscentedKalmanFilterTest::getDynamics,
                  UnscentedKalmanFilterTest::getLocalMeasurementModel,
                  VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                  VecBuilder.fill(0.0001, 0.01, 0.01),
                  AngleStatistics.angleMean(2),
                  AngleStatistics.angleMean(0),
                  AngleStatistics.angleResidual(2),
                  AngleStatistics.angleResidual(0),
                  AngleStatistics.angleAdd(2),
                  dtSeconds);

          var u = VecBuilder.fill(12.0, 12.0);
          observer.predict(u, dtSeconds);

          var localY = getLocalMeasurementModel(observer.getXhat(), u);
          observer.correct(u, localY);

          var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
          var R =
              StateSpaceUtil.makeCovarianceMatrix(
                  Nat.N5(), VecBuilder.fill(0.01, 0.01, 0.0001, 0.01, 0.01));
          observer.correct(
              Nat.N5(),
              u,
              globalY,
              UnscentedKalmanFilterTest::getGlobalMeasurementModel,
              R,
              AngleStatistics.angleMean(2),
              AngleStatistics.angleResidual(2),
              AngleStatistics.angleResidual(2),
              AngleStatistics.angleAdd(2));
        });
  }

  @Test
  void testConvergence() {
    double dtSeconds = 0.005;
    double rbMeters = 0.8382 / 2.0; // Robot radius

    UnscentedKalmanFilter<N5, N2, N3> observer =
        new UnscentedKalmanFilter<>(
            Nat.N5(),
            Nat.N3(),
            UnscentedKalmanFilterTest::getDynamics,
            UnscentedKalmanFilterTest::getLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
            VecBuilder.fill(0.0001, 0.5, 0.5),
            AngleStatistics.angleMean(2),
            AngleStatistics.angleMean(0),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleAdd(2),
            dtSeconds);

    List<Pose2d> waypoints =
        List.of(
            new Pose2d(2.75, 22.521, Rotation2d.kZero),
            new Pose2d(24.73, 19.68, Rotation2d.fromDegrees(5.846)));
    var trajectory =
        TrajectoryGenerator.generateTrajectory(waypoints, new TrajectoryConfig(8.8, 0.1));

    Matrix<N5, N1> r = new Matrix<>(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = new Matrix<>(Nat.N2(), Nat.N1());

    var B =
        NumericalJacobian.numericalJacobianU(
            Nat.N5(),
            Nat.N2(),
            UnscentedKalmanFilterTest::getDynamics,
            new Matrix<>(Nat.N5(), Nat.N1()),
            new Matrix<>(Nat.N2(), Nat.N1()));

    observer.setXhat(
        VecBuilder.fill(
            trajectory.getInitialPose().getTranslation().getX(),
            trajectory.getInitialPose().getTranslation().getY(),
            trajectory.getInitialPose().getRotation().getRadians(),
            0.0,
            0.0));

    var trueXhat = observer.getXhat();

    double totalTime = trajectory.getTotalTimeSeconds();
    for (int i = 0; i < (totalTime / dtSeconds); i++) {
      var ref = trajectory.sample(dtSeconds * i);
      double vl = ref.velocityMetersPerSecond * (1 - (ref.curvatureRadPerMeter * rbMeters));
      double vr = ref.velocityMetersPerSecond * (1 + (ref.curvatureRadPerMeter * rbMeters));

      var nextR =
          VecBuilder.fill(
              ref.poseMeters.getTranslation().getX(),
              ref.poseMeters.getTranslation().getY(),
              ref.poseMeters.getRotation().getRadians(),
              vl,
              vr);

      Matrix<N3, N1> localY = getLocalMeasurementModel(trueXhat, new Matrix<>(Nat.N2(), Nat.N1()));
      var noiseStdDev = VecBuilder.fill(0.0001, 0.5, 0.5);

      observer.correct(u, localY.plus(StateSpaceUtil.makeWhiteNoiseVector(noiseStdDev)));

      var rdot = nextR.minus(r).div(dtSeconds);
      u = new Matrix<>(B.solve(rdot.minus(getDynamics(r, new Matrix<>(Nat.N2(), Nat.N1())))));

      observer.predict(u, dtSeconds);

      r = nextR;
      trueXhat =
          NumericalIntegration.rk4(UnscentedKalmanFilterTest::getDynamics, trueXhat, u, dtSeconds);
    }

    var localY = getLocalMeasurementModel(trueXhat, u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(trueXhat, u);
    var R =
        StateSpaceUtil.makeCovarianceMatrix(
            Nat.N5(), VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(
        Nat.N5(),
        u,
        globalY,
        UnscentedKalmanFilterTest::getGlobalMeasurementModel,
        R,
        AngleStatistics.angleMean(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleAdd(2));

    final var finalPosition = trajectory.sample(trajectory.getTotalTimeSeconds());

    assertEquals(finalPosition.poseMeters.getTranslation().getX(), observer.getXhat(0), 0.055);
    assertEquals(finalPosition.poseMeters.getTranslation().getY(), observer.getXhat(1), 0.15);
    assertEquals(
        finalPosition.poseMeters.getRotation().getRadians(), observer.getXhat(2), 0.000005);
    assertEquals(0.0, observer.getXhat(3), 0.1);
    assertEquals(0.0, observer.getXhat(4), 0.1);
  }

  @Test
  void testLinearUKF() {
    var dt = 0.020;
    var plant = LinearSystemId.identifyVelocitySystem(0.02, 0.006);
    var observer =
        new UnscentedKalmanFilter<>(
            Nat.N1(),
            Nat.N1(),
            (x, u) -> plant.getA().times(x).plus(plant.getB().times(u)),
            plant::calculateY,
            VecBuilder.fill(0.05),
            VecBuilder.fill(1.0),
            dt);

    var discABPair = Discretization.discretizeAB(plant.getA(), plant.getB(), dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    Matrix<N1, N1> ref = VecBuilder.fill(100);
    Matrix<N1, N1> u = VecBuilder.fill(0);

    for (int i = 0; i < (2.0 / dt); i++) {
      observer.predict(u, dt);

      u = discB.solve(ref.minus(discA.times(ref)));
    }

    assertEquals(ref.get(0, 0), observer.getXhat(0), 5);
  }

  @Test
  void testRoundTripP() {
    var dtSeconds = 0.005;

    var observer =
        new UnscentedKalmanFilter<>(
            Nat.N2(),
            Nat.N2(),
            (x, u) -> x,
            (x, u) -> x,
            VecBuilder.fill(0.0, 0.0),
            VecBuilder.fill(0.0, 0.0),
            dtSeconds);

    var P = MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 1.0, 1.0, 2.0);
    observer.setP(P);

    assertTrue(observer.getP().isEqual(P, 1e-9));
  }
}
