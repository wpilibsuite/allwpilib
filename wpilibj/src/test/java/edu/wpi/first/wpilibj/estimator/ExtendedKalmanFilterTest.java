package edu.wpi.first.wpilibj.estimator;

import java.util.Arrays;
import java.util.List;

import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpilibj.system.NumericalJacobian;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N5;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;

@SuppressWarnings("ParameterName")
public class ExtendedKalmanFilterTest {
  public static Matrix<N5, N1> getDynamics(final Matrix<N5, N1> x, final Matrix<N2, N1> u) {
    final var motors = DCMotor.getCIM(2);

    final var gr = 7.08; // Gear ratio
    final var rb = 0.8382 / 2.0; // Wheelbase radius (track width)
    final var r = 0.0746125; // Wheel radius
    final var m = 63.503; // Robot mass
    final var J = 5.6; // Robot moment of inertia

    final var C1 =
            -Math.pow(gr, 2) * motors.m_KtNMPerAmp / (
                    motors.m_KvRadPerSecPerVolt * motors.m_rOhms * r * r);
    final var C2 = gr * motors.m_KtNMPerAmp / (motors.m_rOhms * r);
    final var k1 = 1.0 / m + rb * rb / J;
    final var k2 = 1.0 / m - rb * rb / J;

    final var vl = x.get(3, 0);
    final var vr = x.get(4, 0);
    final var Vl = u.get(0, 0);
    final var Vr = u.get(1, 0);

    final Matrix<N5, N1> result = new Matrix<>(new SimpleMatrix(5, 1));
    final var v = 0.5 * (vl + vr);
    result.set(0, 0, v * Math.cos(x.get(2, 0)));
    result.set(1, 0, v * Math.sin(x.get(2, 0)));
    result.set(2, 0, (vr - vl) / (2.0 * rb));
    result.set(3, 0, k1 * ((C1 * vl) + (C2 * Vl)) + k2 * ((C1 * vr) + (C2 * Vr)));
    result.set(4, 0, k2 * ((C1 * vl) + (C2 * Vl)) + k1 * ((C1 * vr) + (C2 * vr)));
    return result;
  }

  public static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  public static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(
            x.get(0, 0),
            x.get(1, 0),
            x.get(2, 0),
            x.get(3, 0),
            x.get(4, 0)
    );
  }

  @SuppressWarnings("LocalVariableName")
  @Test
  public void testInit() {
    double dtSeconds = 0.00505;

    assertDoesNotThrow(() -> {
      ExtendedKalmanFilter<N5, N2, N3> observer =
              new ExtendedKalmanFilter<>(Nat.N5(), Nat.N2(), Nat.N3(),
                      ExtendedKalmanFilterTest::getDynamics,
                      ExtendedKalmanFilterTest::getLocalMeasurementModel,
                      VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                      VecBuilder.fill(0.0001, 0.01, 0.01), dtSeconds);

      Matrix<N2, N1> u = VecBuilder.fill(12.0, 12.0);
      observer.predict(u, dtSeconds);

      var localY = getLocalMeasurementModel(observer.getXhat(), u);
      observer.correct(u, localY);

      var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
      var R = StateSpaceUtils.makeCostMatrix(
              VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
      observer.correct(Nat.N5(),
              u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
    });

  }

  @SuppressWarnings("LocalVariableName")
  @Test
  public void testConvergence() {
    double dtSeconds = 0.00505;
    double rbMeters = 0.8382 / 2.0; // Robot radius

    ExtendedKalmanFilter<N5, N2, N3> observer =
            new ExtendedKalmanFilter<>(Nat.N5(), Nat.N2(), Nat.N3(),
                    ExtendedKalmanFilterTest::getDynamics,
                    ExtendedKalmanFilterTest::getLocalMeasurementModel,
                    VecBuilder.fill(0.5, 0.5, 10.0, 1.0, 1.0),
                    VecBuilder.fill(0.001, 0.01, 0.01), dtSeconds);

    List<Pose2d> waypoints = Arrays.asList(new Pose2d(2.75, 22.521, new Rotation2d()),
            new Pose2d(24.73, 19.68, Rotation2d.fromDegrees(5.846)));
    var trajectory = TrajectoryGenerator.generateTrajectory(
            waypoints,
            new TrajectoryConfig(8.8, 0.1)
    );

    Matrix<N5, N1> r = MatrixUtils.zeros(Nat.N5(), Nat.N1());

    Matrix<N5, N1> nextR = MatrixUtils.zeros(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = MatrixUtils.zeros(Nat.N2(), Nat.N1());

    var B = NumericalJacobian.numericalJacobianU(Nat.N5(), Nat.N2(),
            ExtendedKalmanFilterTest::getDynamics, MatrixUtils.zeros(Nat.N5(), Nat.N1()), u);

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

      var localY =
              getLocalMeasurementModel(observer.getXhat(), MatrixUtils.zeros(Nat.N2(), Nat.N1()));
      var whiteNoiseStdDevs = VecBuilder.fill(0.0001, 0.5, 0.5);
      observer.correct(u,
              localY.plus(StateSpaceUtils.makeWhiteNoiseVector(Nat.N3(), whiteNoiseStdDevs)));

      Matrix<N5, N1> rdot = nextR.minus(r).div(dtSeconds);
      u = new Matrix<>(SimpleMatrixUtils.householderQrDecompose(B.getStorage())
            .solve(rdot.minus(getDynamics(r, MatrixUtils.zeros(Nat.N2(), Nat.N1()))).getStorage()));

      observer.predict(u, dtSeconds);

      r = nextR;
    }

    var localY = getLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
    var R = StateSpaceUtils.makeCostMatrix(
            VecBuilder.fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
  }
}
