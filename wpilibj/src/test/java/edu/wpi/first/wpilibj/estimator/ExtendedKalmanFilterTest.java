package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.geometry.*;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.system.NumericalJacobian;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
import java.util.function.BiFunction;

public class ExtendedKalmanFilterTest {
  public static Matrix<N5, N1> getDynamics(final Matrix<N5, N1> x, final Matrix<N2, N1> u) {
    final var motors = DCMotor.getCIM(2);

    final var gr = 7.08; // Gear ratio
    final var rb = 0.8382 / 2.0; // Wheelbase radius (track width)
    final var r = 0.0746125; // Wheel radius
    final var m = 63.503; // Robot mass
    final var J = 5.6; // Robot moment of inertia

    final var C1 = -Math.pow(gr, 2) * motors.m_KtNMPerAmp / (motors.m_KvRadPerSecPerVolt * motors.m_rOhms * r * r);
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
    result.set(2, 0, ((vr - vl) / (2.0 * rb)));
    result.set(3, 0, k1 * ((C1 * vl) + (C2 * Vl)) + k2 * ((C1 * vr) + (C2 * Vr)));
    result.set(4, 0, k2 * ((C1 * vl) + (C2 * Vl)) + k1 * ((C1 * vr) + (C2 * vr)));
    return result;
  }

  public static Matrix<N3, N1> getLocalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return new MatBuilder<>(Nat.N3(), Nat.N1()).fill(x.get(2, 0), x.get(3, 0), x.get(4, 0));
  }

  public static Matrix<N5, N1> getGlobalMeasurementModel(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    return new MatBuilder<>(Nat.N5(), Nat.N1()).fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(3, 0),
            x.get(4, 0));
  }

  @Test
  public void testInit() {
    double dtSeconds = 0.00505;

    ExtendedKalmanFilter<N5, N2, N3> observer = new ExtendedKalmanFilter<>(Nat.N5(), Nat.N2(), Nat.N3(),
            UnscentedKalmanFilterTest::getDynamics, UnscentedKalmanFilterTest::getLocalMeasurementModel,
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.5, 0.5, 10.0, 1.0, 1.0),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.01, 0.01), true, dtSeconds);

    Matrix<N2, N1> u = new MatBuilder<N2, N1>(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    observer.predict(u, dtSeconds);

    var localY = getLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
    var R = StateSpaceUtils.makeCostMatrix(Nat.N5(),
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
  }

  @Test
  public void testConvergence() {
    double dtSeconds = 0.00505;
    double rbMeters = 0.8382 / 2.0; // Robot radius

    ExtendedKalmanFilter<N5, N2, N3> observer = new ExtendedKalmanFilter<>(Nat.N5(), Nat.N2(), Nat.N3(),
            UnscentedKalmanFilterTest::getDynamics, UnscentedKalmanFilterTest::getLocalMeasurementModel,
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.5, 0.5, 10.0, 1.0, 1.0),
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.001, 0.01, 0.01), true, dtSeconds);

    List<Pose2d> waypoints = Arrays.asList(new Pose2d(2.75, 22.521, new Rotation2d()),
            new Pose2d(24.73, 19.68, Rotation2d.fromDegrees(5.846)));
    var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, new TrajectoryConfig(8.8, 0.1));

    Matrix<N5, N1> r = MatrixUtils.zeros(Nat.N5(), Nat.N1());

    Matrix<N5, N1> nextR = MatrixUtils.zeros(Nat.N5(), Nat.N1());
    Matrix<N2, N1> u = MatrixUtils.zeros(Nat.N2(), Nat.N1());

    var B = NumericalJacobian.numericalJacobianU(Nat.N5(), Nat.N2(), ExtendedKalmanFilterTest::getDynamics,
            MatrixUtils.zeros(Nat.N5(), Nat.N1()), u);

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

      var localY = getLocalMeasurementModel(observer.getXhat(), MatrixUtils.zeros(Nat.N2(), Nat.N1()));
      var whiteNoiseStdDevs = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.5, 0.5);
      observer.correct(u, localY.plus(StateSpaceUtils.makeWhiteNoiseVector(Nat.N3(), whiteNoiseStdDevs)));

      Matrix<N5, N1> rdot = nextR.minus(r).div(dtSeconds);
      u = new Matrix<>(StateSpaceUtils.householderQrDecompose(B.getStorage())
              .solve(rdot.minus(getDynamics(r, MatrixUtils.zeros(Nat.N2(), Nat.N1()))).getStorage()));

      observer.predict(u, dtSeconds);

      r = nextR;
    }

    var localY = getLocalMeasurementModel(observer.getXhat(), u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(observer.getXhat(), u);
    var R = StateSpaceUtils.makeCostMatrix(Nat.N5(),
            new MatBuilder<>(Nat.N5(), Nat.N1()).fill(0.01, 0.01, 0.0001, 0.5, 0.5));
    observer.correct(Nat.N5(), u, globalY, ExtendedKalmanFilterTest::getGlobalMeasurementModel, R);
  }

  @Test
  public void testLocalization() {
    /**
     * Our state-space system is:
     *
     * x = [[x, y, dtheta]]^T in the field coordinate system
     *
     * u = [[v_l, v_r, theta]]^T Not actual inputs, but the dynamics math required
     * to use actual inputs like voltage is gross.
     *
     * y = [[x_s, y_s, theta_s, x_r, y_r, theta_r]]^T All the subscript s ones are
     * measurements from vSLAM, while the sub r ones are from retroreflective tape
     * vision. Most people probably only have the retroreflective tape PnP
     * measurements, but my setup is a little extra.
     */

    final double dt = 0.01; // The (nominal) loop time of the observer
    final double visionUpdateRate = 0.1;

    BiFunction<Matrix<N3, N1>, Matrix<N3, N1>, Matrix<N3, N1>> f = (x, u) -> {
      // Diff drive forward kinematics:
      // v_c = (v_l + v_r) / 2
      var newPose = new Pose2d(x.get(0, 0), x.get(1, 0), new Rotation2d(x.get(2, 0)))
              .exp(new Twist2d((u.get(0, 0) + u.get(1, 0)) / 2, 0.0, u.get(2, 0)));

      return new MatBuilder<>(Nat.N3(), Nat.N1()).fill(newPose.getTranslation().getX(),
              newPose.getTranslation().getY(), x.get(2, 0) + u.get(2, 0));
    };

    BiFunction<Matrix<N3, N1>, Matrix<N3, N1>, Matrix<N6, N1>> h = (x, u) -> {
      return new MatBuilder<>(Nat.N6(), Nat.N1()).fill(x.get(0, 0), x.get(1, 0), x.get(2, 0), x.get(0, 0),
              x.get(1, 0), x.get(2, 0));
    };

    var stateStdDevs = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.002, 0.002, 0.001);
    var withVisionEkf = new ExtendedKalmanFilter<>(Nat.N3(), Nat.N3(), Nat.N6(), f, h,
            stateStdDevs,
            new MatBuilder<>(Nat.N6(), Nat.N1()).fill(0.05, 0.05, 0.001, 0.05, 0.05, 0.02), false, dt);
    var ekf = new ExtendedKalmanFilter<N3, N3, N3>(Nat.N3(), Nat.N3(), Nat.N3(), f, (x, u) -> x,
            stateStdDevs,
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.05, 0.05, 0.001), false, dt);

    var traj = TrajectoryGenerator.generateTrajectory(
            List.of(
                    new Pose2d(), new Pose2d(20, 20, Rotation2d.fromDegrees(0)),
                    new Pose2d(23, 23, Rotation2d.fromDegrees(173)), new Pose2d(54, 54, new Rotation2d())
            ), new TrajectoryConfig(0.5, 2));

    var kinematics = new DifferentialDriveKinematics(1);
    Pose2d lastPose = null;

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> slamXs = new ArrayList<>();
    List<Double> slamYs = new ArrayList<>();
    List<Double> visionXs = new ArrayList<>();
    List<Double> visionYs = new ArrayList<>();

    double t = 0.0;
    double lastVisionUpdate = Double.NEGATIVE_INFINITY;
    double maxError = Double.NEGATIVE_INFINITY;
    double errorSum = 0;
    while (t <= traj.getTotalTimeSeconds()) {
      var groundtruthState = traj.sample(t);
      var input = kinematics.toWheelSpeeds(new ChassisSpeeds(groundtruthState.velocityMetersPerSecond, 0.0,
              // ds/dt * dtheta/ds = dtheta/dt
              groundtruthState.velocityMetersPerSecond * groundtruthState.curvatureRadPerMeter));

      Matrix<N3, N1> u = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(input.leftMetersPerSecond * dt,
              input.rightMetersPerSecond * dt, 0.0);
      if (lastPose != null) {
        u.set(2, 0,
                groundtruthState.poseMeters.getRotation().getRadians() - lastPose.getRotation().getRadians());
      }
      u = u.plus(StateSpaceUtils.makeWhiteNoiseVector(Nat.N3(), new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.002, 0.002, 0.001)));

      var measurementRetroreflective = groundtruthState.poseMeters;
      var measurementVSlam = groundtruthState.poseMeters;

      if (lastVisionUpdate + visionUpdateRate < t && groundtruthState.poseMeters.getTranslation().getX() < 54 / 2) {
        lastVisionUpdate = t;
        withVisionEkf.setP(ekf.getP());
        withVisionEkf.setXhat(ekf.getXhat());

        var y = new MatBuilder<>(Nat.N6(), Nat.N1()).fill(
                measurementVSlam.getTranslation().getX(),
                measurementVSlam.getTranslation().getY(),
                measurementVSlam.getRotation().getRadians(),
                measurementRetroreflective.getTranslation().getX(),
                measurementRetroreflective.getTranslation().getY(),
                measurementRetroreflective.getRotation().getRadians());
        y = y.plus(StateSpaceUtils.makeWhiteNoiseVector(Nat.N6(), new MatBuilder<>(Nat.N6(), Nat.N1()).fill(0.05, 0.05, 0.001, 0.05, 0.05, 0.02)));

        withVisionEkf.correct(u, y);
        withVisionEkf.predict(u, dt);

        ekf.setP(withVisionEkf.getP());
        ekf.setXhat(withVisionEkf.getXhat());

        slamXs.add(y.get(0, 0));
        slamYs.add(y.get(1, 0));
        visionXs.add(y.get(3, 0));
        visionYs.add(y.get(4, 0));
      } else {
        var y = new MatBuilder<>(Nat.N3(), Nat.N1()).fill(
                measurementVSlam.getTranslation().getX(),
                measurementVSlam.getTranslation().getY(),
                measurementVSlam.getRotation().getRadians()
        );
        y = y.plus(StateSpaceUtils.makeWhiteNoiseVector(Nat.N3(), new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.05, 0.05, 0.001)));

        ekf.correct(u, y);
        ekf.predict(u, dt);

        slamXs.add(y.get(0, 0));
        slamYs.add(y.get(1, 0));
      }

      double error = groundtruthState.poseMeters.getTranslation().getDistance(new Translation2d(ekf.getXhat(0), ekf.getXhat(1)));
      if (error > maxError) {
        maxError = error;
      }
      errorSum += error;

      trajXs.add(groundtruthState.poseMeters.getTranslation().getX());
      trajYs.add(groundtruthState.poseMeters.getTranslation().getY());
      observerXs.add(ekf.getXhat(0));
      observerYs.add(ekf.getXhat(1));

      lastPose = groundtruthState.poseMeters;
      t += dt;
    }

//        var chartBuilder = new XYChartBuilder();
//        chartBuilder.title = "The Magic of Sensor Fusion";
//        var chart = chartBuilder.build();
//
//        chart.addSeries("vSLAM", slamXs, slamYs);
//        chart.addSeries("Vision", visionXs, visionYs);
//        chart.addSeries("Trajectory", trajXs, trajYs);
//        chart.addSeries("xHat", observerXs, observerYs);

    System.out.println("Mean error (meters): " + errorSum / (traj.getTotalTimeSeconds() / dt));
    System.out.println("Max error (meters):  " + maxError);

//        new SwingWrapper<>(chart).displayChart();
//        try {
//            Thread.sleep(1000000000);
//        } catch (InterruptedException e) {
//        }
  }

  private Transform2d getRandomTransform(double xStdDev, double yStdDev, double thetaStdDev, Random rand) {
    return new Transform2d(new Translation2d(rand.nextGaussian() * xStdDev, rand.nextGaussian() * yStdDev), new Rotation2d(rand.nextGaussian() * thetaStdDev));
  }
}