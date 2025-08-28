// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.Nat;
import org.wpilib.math.util.StateSpaceUtil;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.numbers.N4;
import org.wpilib.math.numbers.N5;
import org.wpilib.math.system.Discretization;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.system.NumericalJacobian;
import org.wpilib.math.system.plant.DCMotor;
import org.wpilib.math.system.plant.LinearSystemId;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.trajectory.TrajectoryGenerator;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import org.junit.jupiter.api.Test;

class MerweUKFTest {
  private static Matrix<N5, N1> driveDynamics(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    var motors = DCMotor.getCIM(2);

    // var gLow = 15.32;    // Low gear ratio
    var gHigh = 7.08; // High gear ratio
    var rb = 0.8382 / 2.0; // Robot radius
    var r = 0.0746125; // Wheel radius
    var m = 63.503; // Robot mass
    var J = 5.6; // Robot moment of inertia

    var C1 = -Math.pow(gHigh, 2) * motors.Kt / (motors.Kv * motors.R * r * r);
    var C2 = gHigh * motors.Kt / (motors.R * r);
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
  private static Matrix<N3, N1> driveLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N5, N1> driveGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return x.copy();
  }

  @Test
  void testDriveInit() {
    var dt = 0.005;
    assertDoesNotThrow(
        () -> {
          MerweUKF<N5, N2, N3> observer =
              new MerweUKF<>(
                  Nat.N5(),
                  Nat.N3(),
                  MerweUKFTest::driveDynamics,
                  MerweUKFTest::driveLocalMeasurementModel,
                  VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                  VecBuilder.fill(0.0001, 0.01, 0.01),
                  AngleStatistics.angleMean(2),
                  AngleStatistics.angleMean(0),
                  AngleStatistics.angleResidual(2),
                  AngleStatistics.angleResidual(0),
                  AngleStatistics.angleAdd(2),
                  dt);

          var u = VecBuilder.fill(12.0, 12.0);
          observer.predict(u, dt);

          var localY = driveLocalMeasurementModel(observer.getXhat(), u);
          observer.correct(u, localY);

          var globalY = driveGlobalMeasurementModel(observer.getXhat(), u);
          var R =
              StateSpaceUtil.makeCovarianceMatrix(
                  Nat.N5(), VecBuilder.fill(0.01, 0.01, 0.0001, 0.01, 0.01));
          observer.correct(
              Nat.N5(),
              u,
              globalY,
              MerweUKFTest::driveGlobalMeasurementModel,
              R,
              AngleStatistics.angleMean(2),
              AngleStatistics.angleResidual(2),
              AngleStatistics.angleResidual(2),
              AngleStatistics.angleAdd(2));
        });
  }

  @Test
  void testDriveConvergence() {
    final double dt = 0.005;
    final double rb = 0.8382 / 2.0; // Robot radius

    MerweUKF<N5, N2, N3> observer =
        new MerweUKF<>(
            Nat.N5(),
            Nat.N3(),
            MerweUKFTest::driveDynamics,
            MerweUKFTest::driveLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
            VecBuilder.fill(0.0001, 0.5, 0.5),
            AngleStatistics.angleMean(2),
            AngleStatistics.angleMean(0),
            AngleStatistics.angleResidual(2),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleAdd(2),
            dt);

    List<Pose2d> waypoints =
        List.of(
            new Pose2d(2.75, 22.521, Rotation2d.kZero),
            new Pose2d(24.73, 19.68, Rotation2d.fromRadians(5.846)));
    var trajectory =
        TrajectoryGenerator.generateTrajectory(waypoints, new TrajectoryConfig(8.8, 0.1));

    Matrix<N5, N1> r = new Matrix<>(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = new Matrix<>(Nat.N2(), Nat.N1());

    var B =
        NumericalJacobian.numericalJacobianU(
            Nat.N5(),
            Nat.N2(),
            MerweUKFTest::driveDynamics,
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

    double totalTime = trajectory.getTotalTime();
    for (int i = 0; i < (totalTime / dt); ++i) {
      var ref = trajectory.sample(dt * i);
      double vl = ref.velocity * (1 - (ref.curvature * rb));
      double vr = ref.velocity * (1 + (ref.curvature * rb));

      var nextR =
          VecBuilder.fill(
              ref.pose.getTranslation().getX(),
              ref.pose.getTranslation().getY(),
              ref.pose.getRotation().getRadians(),
              vl,
              vr);

      Matrix<N3, N1> localY =
          driveLocalMeasurementModel(trueXhat, new Matrix<>(Nat.N2(), Nat.N1()));
      var noiseStdDev = VecBuilder.fill(0.0001, 0.5, 0.5);

      observer.correct(u, localY.plus(StateSpaceUtil.makeWhiteNoiseVector(noiseStdDev)));

      var rdot = nextR.minus(r).div(dt);
      u = new Matrix<>(B.solve(rdot.minus(driveDynamics(r, new Matrix<>(Nat.N2(), Nat.N1())))));

      observer.predict(u, dt);

      r = nextR;
      trueXhat = NumericalIntegration.rk4(MerweUKFTest::driveDynamics, trueXhat, u, dt);
    }

    var localY = driveLocalMeasurementModel(trueXhat, u);
    observer.correct(u, localY);

    var globalY = driveGlobalMeasurementModel(trueXhat, u);
    var R =
        StateSpaceUtil.makeCovarianceMatrix(
            Nat.N5(), VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(
        Nat.N5(),
        u,
        globalY,
        MerweUKFTest::driveGlobalMeasurementModel,
        R,
        AngleStatistics.angleMean(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleResidual(2),
        AngleStatistics.angleAdd(2));

    final var finalPosition = trajectory.sample(trajectory.getTotalTime());

    assertEquals(finalPosition.pose.getTranslation().getX(), observer.getXhat(0), 0.055);
    assertEquals(finalPosition.pose.getTranslation().getY(), observer.getXhat(1), 0.15);
    assertEquals(finalPosition.pose.getRotation().getRadians(), observer.getXhat(2), 0.000005);
    assertEquals(0.0, observer.getXhat(3), 0.1);
    assertEquals(0.0, observer.getXhat(4), 0.1);
  }

  @Test
  void testLinearUKF() {
    var dt = 0.020;
    var plant = LinearSystemId.identifyVelocitySystem(0.02, 0.006);
    var observer =
        new MerweUKF<>(
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

    for (int i = 0; i < (2.0 / dt); ++i) {
      observer.predict(u, dt);

      u = discB.solve(ref.minus(discA.times(ref)));
    }

    assertEquals(ref.get(0, 0), observer.getXhat(0), 5);
  }

  @Test
  void testRoundTripP() {
    var dt = 0.005;

    var observer =
        new MerweUKF<>(
            Nat.N2(),
            Nat.N2(),
            (x, u) -> x,
            (x, u) -> x,
            VecBuilder.fill(0.0, 0.0),
            VecBuilder.fill(0.0, 0.0),
            dt);

    var P = MatBuilder.fill(Nat.N2(), Nat.N2(), 2.0, 1.0, 1.0, 2.0);
    observer.setP(P);

    assertTrue(observer.getP().isEqual(P, 1e-9));
  }

  // Second system, single motor feedforward estimator
  private static Matrix<N4, N1> motorDynamics(Matrix<N4, N1> x, Matrix<N1, N1> u) {
    double v = x.get(1, 0);
    double kV = x.get(2, 0);
    double kA = x.get(3, 0);

    double V = u.get(0, 0);

    double a = -kV / kA * v + 1.0 / kA * V;
    return MatBuilder.fill(Nat.N4(), Nat.N1(), v, a, 0, 0);
  }

  private static Matrix<N3, N1> motorMeasurementModel(Matrix<N4, N1> x, Matrix<N1, N1> u) {
    double p = x.get(0, 0);
    double v = x.get(1, 0);
    double kV = x.get(2, 0);
    double kA = x.get(3, 0);
    double V = u.get(0, 0);

    double a = -kV / kA * v + 1.0 / kA * V;
    return MatBuilder.fill(Nat.N3(), Nat.N1(), p, v, a);
  }

  private static Matrix<N1, N1> motorControlInput(double t) {
    return MatBuilder.fill(
        Nat.N1(),
        Nat.N1(),
        Math.clamp(
            8 * Math.sin(Math.PI * Math.sqrt(2.0) * t)
                + 6 * Math.sin(Math.PI * Math.sqrt(3.0) * t)
                + 4 * Math.sin(Math.PI * Math.sqrt(5.0) * t),
            -12.0,
            12.0));
  }

  @Test
  void testMotorConvergence() {
    final double dt = 0.01;
    final int steps = 500;
    final double true_kV = 3;
    final double true_kA = 0.2;

    final double pos_stddev = 0.02;
    final double vel_stddev = 0.1;
    final double accel_stddev = 0.1;

    var states =
        new ArrayList<>(
            Collections.nCopies(
                steps + 1, MatBuilder.fill(Nat.N4(), Nat.N1(), 0.0, 0.0, 0.0, 0.0)));
    var inputs =
        new ArrayList<>(Collections.nCopies(steps, MatBuilder.fill(Nat.N1(), Nat.N1(), 0.0)));
    var measurements =
        new ArrayList<>(
            Collections.nCopies(steps + 1, MatBuilder.fill(Nat.N3(), Nat.N1(), 0.0, 0.0, 0.0)));
    states.set(0, MatBuilder.fill(Nat.N4(), Nat.N1(), 0.0, 0.0, true_kV, true_kA));

    var A =
        MatBuilder.fill(
            Nat.N4(),
            Nat.N4(),
            0.0,
            1.0,
            0.0,
            0.0,
            0.0,
            -true_kV / true_kA,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0);
    var B = MatBuilder.fill(Nat.N4(), Nat.N1(), 0.0, 1.0 / true_kA, 0.0, 0.0);

    var discABPair = Discretization.discretizeAB(A, B, dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    for (int i = 0; i < steps; ++i) {
      inputs.set(i, motorControlInput(i * dt));
      states.set(i + 1, discA.times(states.get(i)).plus(discB.times(inputs.get(i))));
      measurements.set(
          i,
          motorMeasurementModel(states.get(i + 1), inputs.get(i))
              .plus(
                  StateSpaceUtil.makeWhiteNoiseVector(
                      VecBuilder.fill(pos_stddev, vel_stddev, accel_stddev))));
    }

    var P0 =
        MatBuilder.fill(
            Nat.N4(), Nat.N4(), 0.001, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 0.0, 0.0, 0.0, 10, 0.0, 0.0,
            0.0, 0.0, 10);

    var observer =
        new MerweUKF<N4, N1, N3>(
            Nat.N4(),
            Nat.N3(),
            MerweUKFTest::motorDynamics,
            MerweUKFTest::motorMeasurementModel,
            VecBuilder.fill(0.1, 1.0, 1e-10, 1e-10),
            VecBuilder.fill(pos_stddev, vel_stddev, accel_stddev),
            dt);

    observer.setXhat(MatBuilder.fill(Nat.N4(), Nat.N1(), 0.0, 0.0, 2.0, 2.0));
    observer.setP(P0);

    for (int i = 0; i < steps; ++i) {
      observer.predict(inputs.get(i), dt);
      observer.correct(inputs.get(i), measurements.get(i));
    }

    assertEquals(true_kV, observer.getXhat(2), true_kV * 0.5);
    assertEquals(true_kA, observer.getXhat(3), true_kA * 0.5);
  }
}
