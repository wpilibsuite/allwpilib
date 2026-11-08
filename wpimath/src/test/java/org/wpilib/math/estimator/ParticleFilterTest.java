// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.Random;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N3;
import org.wpilib.math.numbers.N5;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.system.NumericalJacobian;
import org.wpilib.math.trajectory.DrivetrainSplineTrajectoryGenerator;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.util.MathUtil;
import org.wpilib.math.util.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.math.util.StateSpaceUtil;

class ParticleFilterTest {
  private static final long kSeed = 42;

  private static final DCMotor kMotors = DCMotor.getCIM(2);

  private static Matrix<N5, N1> getDrivetrainDynamics(
      final Matrix<N5, N1> x, final Matrix<N2, N1> u) {
    final var gr = 7.08; // Gear ratio
    final var rb = 0.8382 / 2.0; // Wheelbase radius (trackwidth)
    final var r = 0.0746125; // Wheel radius
    final var m = 63.503; // Robot mass
    final var J = 5.6; // Robot moment of inertia

    final var C1 = -Math.pow(gr, 2) * kMotors.Kt / (kMotors.Kv * kMotors.R * r * r);
    final var C2 = gr * kMotors.Kt / (kMotors.R * r);
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
  private static Matrix<N3, N1> getDrivetrainLocalMeasurementModel(
      Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N2, N1> getDrivetrainPositionMeasurementModel(
      Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(0, 0), x.get(1, 0));
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N5, N1> getDrivetrainGlobalMeasurementModel(
      Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  // First-order linear system ẋ = −x + u
  private static Matrix<N1, N1> getLinearDynamics(Matrix<N1, N1> x, Matrix<N1, N1> u) {
    return x.times(-1.0).plus(u);
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N1, N1> getLinearMeasurementModel(Matrix<N1, N1> x, Matrix<N1, N1> u) {
    return x;
  }

  // Heading with a commanded angular velocity θ̇ = u
  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N1, N1> getHeadingDynamics(Matrix<N1, N1> x, Matrix<N1, N1> u) {
    return u;
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private static Matrix<N1, N1> getHeadingMeasurementModel(Matrix<N1, N1> x, Matrix<N1, N1> u) {
    return x;
  }

  // Returns a vector of normally distributed noise from a seeded generator so
  // tests are deterministic
  private static <N extends Num> Matrix<N, N1> seededNoise(Random random, Matrix<N, N1> stdDevs) {
    final var result = stdDevs.copy();
    for (int i = 0; i < stdDevs.getNumRows(); ++i) {
      result.set(i, 0, stdDevs.get(i, 0) * random.nextGaussian());
    }
    return result;
  }

  private static ParticleFilter<N1, N1, N1> makeLinearFilter(
      double stateStdDev, double measurementStdDev, int numParticles, double dt) {
    return new ParticleFilter<>(
        Nat.N1(),
        Nat.N1(),
        ParticleFilterTest::getLinearDynamics,
        ParticleFilterTest::getLinearMeasurementModel,
        VecBuilder.fill(stateStdDev),
        VecBuilder.fill(measurementStdDev),
        numParticles,
        dt);
  }

  private static boolean allFinite(Matrix<?, ?> m) {
    for (int row = 0; row < m.getNumRows(); ++row) {
      for (int col = 0; col < m.getNumCols(); ++col) {
        if (!Double.isFinite(m.get(row, col))) {
          return false;
        }
      }
    }
    return true;
  }

  @Test
  void testInit() {
    double dt = 0.005;

    ParticleFilter<N5, N2, N3> observer =
        new ParticleFilter<>(
            Nat.N5(),
            Nat.N3(),
            ParticleFilterTest::getDrivetrainDynamics,
            ParticleFilterTest::getDrivetrainLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
            VecBuilder.fill(0.0001, 0.01, 0.01),
            100,
            dt);
    assertEquals(100, observer.getNumParticles());

    Matrix<N2, N1> u = VecBuilder.fill(12.0, 12.0);
    observer.predict(u, dt);

    var localY = getDrivetrainLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);

    var globalY = getDrivetrainGlobalMeasurementModel(observer.getXhat(), u);
    var R =
        StateSpaceUtil.covarianceMatrix(Nat.N5(), VecBuilder.fill(0.01, 0.01, 0.0001, 0.01, 0.01));
    observer.correct(
        Nat.N5(), u, globalY, ParticleFilterTest::getDrivetrainGlobalMeasurementModel, R);

    assertTrue(allFinite(observer.getXhat()));
    assertTrue(allFinite(observer.getP()));
    assertEquals(1.0, observer.getWeights().elementSum(), 1e-12);
  }

  @Test
  void testInvalidParticleCount() {
    assertThrows(IllegalArgumentException.class, () -> makeLinearFilter(0.1, 0.1, 0, 0.01));
    assertThrows(IllegalArgumentException.class, () -> makeLinearFilter(0.1, 0.1, -1, 0.01));
  }

  @Test
  void testConvergence() {
    runConvergence(kSeed);
  }

  private static void runConvergence(long seed) {
    double dt = 0.005;
    double rb = 0.8382 / 2.0; // Robot radius

    // ExtendedKalmanFilterTest uses a heading measurement standard deviation of
    // 0.0001, but with a heading process noise this large, almost every particle
    // would be inconsistent with each heading measurement. The filter would then
    // resample down to a handful of ancestors every timestep and lose what it
    // learned about the other states.
    ParticleFilter<N5, N2, N3> observer =
        new ParticleFilter<>(
            Nat.N5(),
            Nat.N3(),
            ParticleFilterTest::getDrivetrainDynamics,
            ParticleFilterTest::getDrivetrainLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
            VecBuilder.fill(0.01, 0.5, 0.5),
            200,
            dt);
    observer.setSeed(seed);
    var random = new Random(seed + 1);

    List<Pose2d> waypoints =
        List.of(
            new Pose2d(2.75, 22.521, Rotation2d.ZERO),
            new Pose2d(24.73, 19.68, Rotation2d.fromRadians(5.846)));
    var trajectory =
        DrivetrainSplineTrajectoryGenerator.generate(waypoints, new TrajectoryConfig(8.8, 0.1));

    Matrix<N5, N1> r = new Matrix<>(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = new Matrix<>(Nat.N2(), Nat.N1());

    var B =
        NumericalJacobian.numericalJacobianU(
            Nat.N5(),
            Nat.N2(),
            ParticleFilterTest::getDrivetrainDynamics,
            new Matrix<>(Nat.N5(), Nat.N1()),
            new Matrix<>(Nat.N2(), Nat.N1()));

    observer.setXhat(
        VecBuilder.fill(
            trajectory.start().pose.getTranslation().getX(),
            trajectory.start().pose.getTranslation().getY(),
            trajectory.start().pose.getRotation().getRadians(),
            0.0,
            0.0));

    // The local measurements don't observe x and y, so without a global
    // measurement their estimates would only dead reckon. A particle filter
    // resamples often, which makes its estimate of an unobserved state wander
    // like a single sample of the process noise random walk instead of tracking
    // the posterior mean like a Kalman filter. Add a noisy position measurement
    // every 100 ms, like a vision system would provide.
    final int kGlobalMeasurementPeriod = 20;
    final var R = StateSpaceUtil.covarianceMatrix(Nat.N2(), VecBuilder.fill(0.01, 0.01));

    double duration = trajectory.duration();
    for (int i = 0; i < (duration / dt); ++i) {
      var ref = trajectory.sampleAt(dt * i);
      double vl = ref.forwardVelocity() * (1 - (ref.curvature * rb));
      double vr = ref.forwardVelocity() * (1 + (ref.curvature * rb));

      Matrix<N5, N1> nextR =
          VecBuilder.fill(
              ref.pose.getTranslation().getX(),
              ref.pose.getTranslation().getY(),
              ref.pose.getRotation().getRadians(),
              vl,
              vr);

      var localY = getDrivetrainLocalMeasurementModel(nextR, new Matrix<>(Nat.N2(), Nat.N1()));
      observer.correct(u, localY.plus(seededNoise(random, VecBuilder.fill(0.0001, 0.5, 0.5))));

      if (i % kGlobalMeasurementPeriod == 0) {
        var globalY =
            getDrivetrainPositionMeasurementModel(nextR, new Matrix<>(Nat.N2(), Nat.N1()))
                .plus(seededNoise(random, VecBuilder.fill(0.05, 0.05)));
        observer.correct(
            Nat.N2(), u, globalY, ParticleFilterTest::getDrivetrainPositionMeasurementModel, R);
      }

      Matrix<N5, N1> rdot = nextR.minus(r).div(dt);
      u =
          new Matrix<>(
              B.solve(rdot.minus(getDrivetrainDynamics(r, new Matrix<>(Nat.N2(), Nat.N1())))));

      observer.predict(u, dt);

      r = nextR;
    }

    var finalPosition = trajectory.sampleAt(trajectory.duration());
    // These tolerances were chosen to pass for many seeds, not just this one
    assertEquals(finalPosition.pose.getTranslation().getX(), observer.getXhat(0), 0.3);
    assertEquals(finalPosition.pose.getTranslation().getY(), observer.getXhat(1), 0.3);
    assertEquals(finalPosition.pose.getRotation().getRadians(), observer.getXhat(2), 0.15);
    assertEquals(0.0, observer.getXhat(3), 1.0);
    assertEquals(0.0, observer.getXhat(4), 1.0);
  }

  @Test
  void testMatchesKalmanFilterOnLinearSystem() {
    runMatchesKalmanFilterOnLinearSystem(kSeed);
  }

  private static void runMatchesKalmanFilterOnLinearSystem(long seed) {
    final double dt = 0.01;
    final double stateStdDev = 0.5;
    final double measurementStdDev = 0.01;

    // An extended Kalman filter is optimal for a linear system with Gaussian
    // noise, so the particle filter should approach it
    var ekf =
        new ExtendedKalmanFilter<N1, N1, N1>(
            Nat.N1(),
            Nat.N1(),
            Nat.N1(),
            ParticleFilterTest::getLinearDynamics,
            ParticleFilterTest::getLinearMeasurementModel,
            VecBuilder.fill(stateStdDev),
            VecBuilder.fill(measurementStdDev),
            dt);
    var pf = makeLinearFilter(stateStdDev, measurementStdDev, 2000, dt);
    pf.setSeed(seed);
    pf.setP(ekf.getP());

    var random = new Random(seed + 1);

    // Discrete process and measurement noise standard deviations for simulating
    // the true system
    final double discStateStdDev = stateStdDev * Math.sqrt(dt);
    final double discMeasurementStdDev = measurementStdDev / Math.sqrt(dt);

    final int kSteps = 500;
    double sumSquaredNormalizedError = 0.0;

    Matrix<N1, N1> x = VecBuilder.fill(0.0);
    for (int i = 0; i < kSteps; ++i) {
      Matrix<N1, N1> u = VecBuilder.fill(Math.sin(i * dt));

      x =
          NumericalIntegration.rk4(ParticleFilterTest::getLinearDynamics, x, u, dt)
              .plus(seededNoise(random, VecBuilder.fill(discStateStdDev)));
      ekf.predict(u, dt);
      pf.predict(u, dt);

      Matrix<N1, N1> y = x.plus(seededNoise(random, VecBuilder.fill(discMeasurementStdDev)));
      ekf.correct(u, y);
      pf.correct(u, y);

      // The estimates should agree to within a fraction of the EKF's standard
      // deviation. The particle filter's Monte Carlo error makes individual
      // timesteps deviate more than the average, so the per-timestep bound is
      // looser than the RMS bound below.
      final double normalizedError = (pf.getXhat(0) - ekf.getXhat(0)) / Math.sqrt(ekf.getP(0, 0));
      assertTrue(Math.abs(normalizedError) < 1.0, "Normalized error " + normalizedError);
      sumSquaredNormalizedError += normalizedError * normalizedError;
    }

    assertTrue(Math.sqrt(sumSquaredNormalizedError / kSteps) < 0.1);
    assertEquals(ekf.getP(0, 0), pf.getP(0, 0), 0.2 * ekf.getP(0, 0));
  }

  @Test
  void testAngleWrapping() {
    runAngleWrapping(kSeed);
  }

  private static void runAngleWrapping(long seed) {
    final double dt = 0.01;

    var pf =
        new ParticleFilter<N1, N1, N1>(
            Nat.N1(),
            Nat.N1(),
            ParticleFilterTest::getHeadingDynamics,
            ParticleFilterTest::getHeadingMeasurementModel,
            VecBuilder.fill(0.1),
            VecBuilder.fill(0.01),
            AngleStatistics.angleMean(0),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleResidual(0),
            AngleStatistics.angleAdd(0),
            500,
            dt);
    pf.setSeed(seed);

    // Start just below π and rotate counterclockwise across the ±π boundary
    double theta = Math.PI - 0.5;
    pf.setXhat(VecBuilder.fill(theta));
    pf.setP(VecBuilder.fill(0.01));

    final Matrix<N1, N1> u = VecBuilder.fill(1.0);
    for (int i = 0; i < 100; ++i) {
      theta = MathUtil.angleModulus(theta + dt);

      pf.predict(u, dt);
      pf.correct(u, VecBuilder.fill(theta));

      // The estimate must stay close to the true heading on the circle, and the
      // particle spread must not blow up from naive arithmetic across the
      // boundary
      final double error =
          AngleStatistics.angleResidual(VecBuilder.fill(theta), pf.getXhat(), 0).get(0, 0);
      assertEquals(0.0, error, 0.05);
      assertTrue(pf.getP(0, 0) < 0.01);
    }

    // The heading wrapped past π
    assertTrue(theta < 0.0);
    assertTrue(pf.getXhat(0) < 0.0);
  }

  @Test
  void testSetPAndSetXhat() {
    runSetPAndSetXhat(kSeed);
  }

  private static void runSetPAndSetXhat(long seed) {
    var pf =
        new ParticleFilter<N2, N1, N1>(
            Nat.N2(),
            Nat.N1(),
            (x, u) -> VecBuilder.fill(x.get(1, 0), 0.0),
            (x, u) -> VecBuilder.fill(x.get(0, 0)),
            VecBuilder.fill(0.1, 0.1),
            VecBuilder.fill(0.1),
            10000,
            0.01);
    pf.setSeed(seed);

    // Particles start at the zero state
    assertEquals(0.0, pf.getXhat().normF(), 1e-12);
    assertEquals(0.0, pf.getP().normF(), 1e-12);

    pf.setXhat(VecBuilder.fill(1.0, 2.0));
    assertEquals(1.0, pf.getXhat(0), 1e-12);
    assertEquals(2.0, pf.getXhat(1), 1e-12);
    assertEquals(0.0, pf.getP().normF(), 1e-12);

    // setP() redraws the particles from a Gaussian with the given covariance,
    // including correlation between states. The tolerances are about five
    // standard errors of the sample moments for 10,000 particles.
    pf.setP(MatBuilder.fill(Nat.N2(), Nat.N2(), 4.0, 1.0, 1.0, 2.0));
    assertEquals(1.0, pf.getXhat(0), 0.1);
    assertEquals(2.0, pf.getXhat(1), 0.1);
    assertEquals(4.0, pf.getP(0, 0), 0.3);
    assertEquals(1.0, pf.getP(0, 1), 0.15);
    assertEquals(1.0, pf.getP(1, 0), 0.15);
    assertEquals(2.0, pf.getP(1, 1), 0.15);

    // setXhat() shifts the particles without changing their spread
    final var oldP = pf.getP().copy();
    pf.setXhat(1, -3.0);
    assertEquals(-3.0, pf.getXhat(1), 1e-12);
    assertEquals(0.0, pf.getP().minus(oldP).normF(), 1e-9);

    // A singular covariance is allowed
    pf.setP(MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 0.0));
    assertEquals(1.0, pf.getP(0, 0), 0.1);
    assertEquals(0.0, pf.getP(1, 1), 1e-12);

    // A singular covariance with perfectly correlated states is allowed. A
    // Cholesky decomposition (LLT) would fail here.
    pf.setP(MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 1.0, 1.0, 1.0));
    assertEquals(1.0, pf.getP(0, 0), 0.1);
    assertEquals(1.0, pf.getP(0, 1), 0.1);
    assertEquals(1.0, pf.getP(1, 1), 0.1);
    final var particles = pf.getParticles();
    final double offset = particles.get(0, 0) - particles.get(1, 0);
    for (int i = 0; i < pf.getNumParticles(); ++i) {
      assertEquals(offset, particles.get(0, i) - particles.get(1, i), 1e-12);
    }

    pf.reset();
    assertEquals(0.0, pf.getXhat().normF(), 1e-12);
    assertEquals(0.0, pf.getP().normF(), 1e-12);
  }

  @Test
  void testResampling() {
    runResampling(kSeed);
  }

  private static void runResampling(long seed) {
    final int N = 1000;
    var pf = makeLinearFilter(0.1, 0.01, N, 0.01);
    pf.setSeed(seed);
    pf.setP(VecBuilder.fill(1.0));

    // A precise measurement makes most particles unlikely, which collapses the
    // effective sample size and triggers resampling back to equal weights
    pf.correct(VecBuilder.fill(0.0), VecBuilder.fill(0.5));
    final var weights = pf.getWeights();
    for (int i = 0; i < N; ++i) {
      assertEquals(1.0 / N, weights.get(i, 0));
    }

    // The exact posterior for a Gaussian prior N(0, P) and measurement noise
    // variance R (here R = 0.01² / 0.01 s after discretization) is
    //
    //   mean = P / (P + R) y
    //   variance = PR / (P + R)
    final double P = 1.0;
    final double R = 0.01;
    assertEquals(P / (P + R) * 0.5, pf.getXhat(0), 0.025);
    assertEquals(P * R / (P + R), pf.getP(0, 0), 0.003);
  }

  @Test
  void testMeasurementInconsistentWithEveryParticle() {
    final int N = 100;
    var pf =
        new ParticleFilter<N1, N1, N1>(
            Nat.N1(),
            Nat.N1(),
            ParticleFilterTest::getLinearDynamics,
            // Every particle produces an invalid measurement
            (x, u) -> VecBuilder.fill(Double.NaN * x.get(0, 0)),
            VecBuilder.fill(0.1),
            VecBuilder.fill(0.01),
            N,
            0.01);
    pf.setSeed(kSeed);
    pf.setP(VecBuilder.fill(1.0));

    final var oldXhat = pf.getXhat().copy();
    pf.correct(VecBuilder.fill(0.0), VecBuilder.fill(0.5));

    // The filter falls back to equal weights instead of producing NaNs
    assertTrue(allFinite(pf.getXhat()));
    assertTrue(allFinite(pf.getP()));
    assertEquals(oldXhat.get(0, 0), pf.getXhat(0));
    final var weights = pf.getWeights();
    for (int i = 0; i < N; ++i) {
      assertEquals(1.0 / N, weights.get(i, 0));
    }
  }

  @Test
  void testSeedMakesOutputReproducible() {
    assertEquals(runSeeded(1), runSeeded(1));
    assertNotEquals(runSeeded(1), runSeeded(2));
  }

  private static double runSeeded(long seed) {
    var pf = makeLinearFilter(0.1, 0.01, 100, 0.01);
    pf.setSeed(seed);
    pf.setP(VecBuilder.fill(1.0));
    for (int i = 0; i < 10; ++i) {
      pf.predict(VecBuilder.fill(1.0), 0.01);
      pf.correct(VecBuilder.fill(1.0), VecBuilder.fill(0.1 * i));
    }
    return pf.getXhat(0);
  }
}
