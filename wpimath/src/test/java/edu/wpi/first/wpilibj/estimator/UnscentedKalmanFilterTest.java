/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.NumericalJacobian;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N4;
import edu.wpi.first.wpiutil.math.numbers.N6;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;


public class UnscentedKalmanFilterTest {
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public static Matrix<N6, N1> getDynamics(Matrix<N6, N1> x, Matrix<N2, N1> u) {
    var motors = DCMotor.getCIM(2);

    var gHigh = 7.08;
    var rb = 0.8382 / 2.0;
    var r = 0.0746125;
    var m = 63.503;
    var J = 5.6;

    var C1 = -Math.pow(gHigh, 2) * motors.m_KtNMPerAmp
          / (motors.m_KvRadPerSecPerVolt * motors.m_rOhms * r * r);
    var C2 = gHigh * motors.m_KtNMPerAmp / (motors.m_rOhms * r);

    var c = x.get(2, 0);
    var s = x.get(3, 0);
    var vl = x.get(4, 0);
    var vr = x.get(5, 0);

    var Vl = u.get(0, 0);
    var Vr = u.get(1, 0);

    var k1 = 1.0 / m + rb * rb / J;
    var k2 = 1.0 / m - rb * rb / J;

    var xvel = (vl + vr) / 2;
    var w = (vr - vl) / (2.0 * rb);

    return VecBuilder.fill(
          xvel * c,
          xvel * s,
          -s * w,
          c * w,
          k1 * ((C1 * vl) + (C2 * Vl)) + k2 * ((C1 * vr) + (C2 * Vr)),
          k2 * ((C1 * vl) + (C2 * Vl)) + k1 * ((C1 * vr) + (C2 * Vr))
    );
  }

  @SuppressWarnings("ParameterName")
  public static Matrix<N4, N1> getLocalMeasurementModel(Matrix<N6, N1> x, Matrix<N2, N1> u) {
    return VecBuilder.fill(x.get(2, 0), x.get(3, 0), x.get(4, 0), x.get(5, 0));
  }

  @SuppressWarnings("ParameterName")
  public static Matrix<N6, N1> getGlobalMeasurementModel(Matrix<N6, N1> x, Matrix<N2, N1> u) {
    return x.copy();
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testInit() {
    assertDoesNotThrow(() -> {
      UnscentedKalmanFilter<N6, N2, N4> observer = new UnscentedKalmanFilter<>(
            Nat.N6(), Nat.N4(),
            UnscentedKalmanFilterTest::getDynamics,
            UnscentedKalmanFilterTest::getLocalMeasurementModel,
            VecBuilder.fill(0.5, 0.5, 0.7, 0.7, 1.0, 1.0),
            VecBuilder.fill(0.001, 0.001, 0.5, 0.5),
            0.00505);

      var u = VecBuilder.fill(12.0, 12.0);
      observer.predict(u, 0.00505);

      var localY = getLocalMeasurementModel(observer.getXhat(), u);
      observer.correct(u, localY);
    });
  }

  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops",
        "PMD.ExcessiveMethodLength"})
  @Test
  public void testConvergence() {
    double dtSeconds = 0.00505;
    double rbMeters = 0.8382 / 2.0; // Robot radius

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();

    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> observerC = new ArrayList<>();
    List<Double> observerS = new ArrayList<>();
    List<Double> observervl = new ArrayList<>();
    List<Double> observervr = new ArrayList<>();

    List<Double> inputVl = new ArrayList<>();
    List<Double> inputVr = new ArrayList<>();

    List<Double> timeData = new ArrayList<>();
    List<Matrix<?, ?>> rdots = new ArrayList<>();

    UnscentedKalmanFilter<N6, N2, N4> observer = new UnscentedKalmanFilter<>(
          Nat.N6(), Nat.N4(),
          UnscentedKalmanFilterTest::getDynamics,
          UnscentedKalmanFilterTest::getLocalMeasurementModel,
          VecBuilder.fill(0.5, 0.5, 0.7, 0.7, 1.0, 1.0),
          VecBuilder.fill(0.001, 0.001, 0.5, 0.5),
          dtSeconds);

    List<Pose2d> waypoints = Arrays.asList(new Pose2d(2.75, 22.521, new Rotation2d()),
          new Pose2d(24.73, 19.68, Rotation2d.fromDegrees(5.846)));
    var trajectory = TrajectoryGenerator.generateTrajectory(
          waypoints,
          new TrajectoryConfig(8.8, 0.1)
    );

    Matrix<N6, N1> nextR;
    Matrix<N2, N1> u = new Matrix<>(Nat.N2(), Nat.N1());

    var B = NumericalJacobian.numericalJacobianU(Nat.N6(), Nat.N2(),
          UnscentedKalmanFilterTest::getDynamics, new Matrix<>(Nat.N6(), Nat.N1()), u);

    observer.setXhat(VecBuilder.fill(2.75, 22.521, 1.0, 0.0, 0.0, 0.0)); // TODO not hard code this

    var ref = trajectory.sample(0.0);

    Matrix<N6, N1> r = VecBuilder.fill(
          ref.poseMeters.getTranslation().getX(),
          ref.poseMeters.getTranslation().getY(),
          ref.poseMeters.getRotation().getCos(),
          ref.poseMeters.getRotation().getSin(),
          ref.velocityMetersPerSecond * (1 - (ref.curvatureRadPerMeter * rbMeters)),
          ref.velocityMetersPerSecond * (1 + (ref.curvatureRadPerMeter * rbMeters))
    );
    nextR = r.copy();

    var trueXhat = observer.getXhat();

    double totalTime = trajectory.getTotalTimeSeconds();
    for (int i = 0; i < (totalTime / dtSeconds); i++) {

      ref = trajectory.sample(dtSeconds * i);
      double vl = ref.velocityMetersPerSecond * (1 - (ref.curvatureRadPerMeter * rbMeters));
      double vr = ref.velocityMetersPerSecond * (1 + (ref.curvatureRadPerMeter * rbMeters));

      nextR.set(0, 0, ref.poseMeters.getTranslation().getX());
      nextR.set(1, 0, ref.poseMeters.getTranslation().getY());
      nextR.set(2, 0, ref.poseMeters.getRotation().getCos());
      nextR.set(3, 0, ref.poseMeters.getRotation().getSin());
      nextR.set(4, 0, vl);
      nextR.set(5, 0, vr);

      Matrix<N4, N1> localY =
            getLocalMeasurementModel(trueXhat, new Matrix<>(Nat.N2(), Nat.N1()));
      var noiseStdDev = VecBuilder.fill(0.001, 0.001, 0.5, 0.5);

      observer.correct(u,
            localY.plus(StateSpaceUtil.makeWhiteNoiseVector(
                  noiseStdDev)));

      var rdot = nextR.minus(r).div(dtSeconds);
      u = new Matrix<>(B.solve(rdot.minus(getDynamics(r, new Matrix<>(Nat.N2(), Nat.N1())))));

      rdots.add(rdot);

      trajXs.add(ref.poseMeters.getTranslation().getX());
      trajYs.add(ref.poseMeters.getTranslation().getY());

      observerXs.add(observer.getXhat().get(0, 0));
      observerYs.add(observer.getXhat().get(1, 0));

      observerC.add(observer.getXhat(2));
      observerS.add(observer.getXhat(3));

      observervl.add(observer.getXhat(4));
      observervr.add(observer.getXhat(5));

      inputVl.add(u.get(0, 0));
      inputVr.add(u.get(1, 0));

      timeData.add(i * dtSeconds);

      r = nextR;
      observer.predict(u, dtSeconds);
      trueXhat = RungeKutta.rungeKutta(UnscentedKalmanFilterTest::getDynamics,
            trueXhat, u, dtSeconds);
    }

    var localY = getLocalMeasurementModel(trueXhat, u);
    observer.correct(u, localY);

    var globalY = getGlobalMeasurementModel(trueXhat, u);
    var R = StateSpaceUtil.makeCostMatrix(
          VecBuilder.fill(0.01, 0.01, 0.0001, 0.0001, 0.5, 0.5));
    observer.correct(Nat.N6(), u, globalY,
        UnscentedKalmanFilterTest::getGlobalMeasurementModel, R,
        (sigmas, weights) -> sigmas.times(Matrix.changeBoundsUnchecked(weights)),
        Matrix::minus, Matrix::minus, Matrix::plus);

    final var finalPosition = trajectory.sample(trajectory.getTotalTimeSeconds());

    //     var chartBuilder = new XYChartBuilder();
    //     chartBuilder.title = "The Magic of Sensor Fusion, now with a "
    //           + observer.getClass().getSimpleName();
    //     var xyPosChart = chartBuilder.build();

    //     xyPosChart.setXAxisTitle("X pos, meters");
    //     xyPosChart.setYAxisTitle("Y pos, meters");
    //     xyPosChart.addSeries("Trajectory", trajXs, trajYs);
    //     xyPosChart.addSeries("xHat", observerXs, observerYs);

    //     var stateChart = new XYChartBuilder()
    //           .title("States (x-hat)").build();
    //     stateChart.addSeries("Cos", timeData, observerC);
    //     stateChart.addSeries("Sin", timeData, observerS);
    //     stateChart.addSeries("vl, m/s", timeData, observervl);
    //     stateChart.addSeries("vr, m/s", timeData, observervr);

    //     var inputChart = new XYChartBuilder().title("Inputs").build();
    //     inputChart.addSeries("Left voltage", timeData, inputVl);
    //     inputChart.addSeries("Right voltage", timeData, inputVr);

    //     var rdotChart = new XYChartBuilder().title("Rdot").build();
    //     rdotChart.addSeries("xdot, or vx", timeData, rdots.stream().map(it -> it.get(0, 0))
    //           .collect(Collectors.toList()));
    //     rdotChart.addSeries("ydot, or vy", timeData, rdots.stream().map(it -> it.get(1, 0))
    //           .collect(Collectors.toList()));
    //     rdotChart.addSeries("cos dot", timeData, rdots.stream().map(it -> it.get(2, 0))
    //           .collect(Collectors.toList()));
    //     rdotChart.addSeries("sin dot", timeData, rdots.stream().map(it -> it.get(3, 0))
    //           .collect(Collectors.toList()));
    //     rdotChart.addSeries("vl dot, or al", timeData, rdots.stream().map(it -> it.get(4, 0))
    //           .collect(Collectors.toList()));
    //     rdotChart.addSeries("vr dot, or ar", timeData, rdots.stream().map(it -> it.get(5, 0))
    //           .collect(Collectors.toList()));

    //     List<XYChart> charts = new ArrayList<>();
    //     charts.add(xyPosChart);
    //     charts.add(stateChart);
    //     charts.add(inputChart);
    //     charts.add(rdotChart);
    //     new SwingWrapper<>(charts).displayChartMatrix();
    //     try {
    //       Thread.sleep(1000000000);
    //     } catch (InterruptedException ex) {
    //       ex.printStackTrace();
    //     }

    assertEquals(finalPosition.poseMeters.getTranslation().getX(), observer.getXhat(0), 0.25);
    assertEquals(finalPosition.poseMeters.getTranslation().getY(), observer.getXhat(1), 0.25);
    assertEquals(finalPosition.poseMeters.getRotation().getRadians(), observer.getXhat(2), 1.0);
    assertEquals(0.0, observer.getXhat(3), 1.0);
    assertEquals(0.0, observer.getXhat(4), 1.0);
  }

  @Test
  @SuppressWarnings({"LocalVariableName", "ParameterName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public void testLinearUKF() {
    var dt = 0.020;
    var plant = LinearSystemId.identifyVelocitySystem(0.02, 0.006);
    var observer = new UnscentedKalmanFilter<>(Nat.N1(), Nat.N1(),
        (x, u) -> plant.getA().times(x).plus(plant.getB().times(u)),
          plant::calculateY,
          VecBuilder.fill(0.05),
          VecBuilder.fill(1.0),
          dt);

    var time = new ArrayList<Double>();
    var refData = new ArrayList<Double>();
    var xhat = new ArrayList<Double>();
    var udata = new ArrayList<Double>();
    var xdotData = new ArrayList<Double>();

    var discABPair = Discretization.discretizeAB(plant.getA(), plant.getB(), dt);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    Matrix<N1, N1> ref = VecBuilder.fill(100);
    Matrix<N1, N1> u = VecBuilder.fill(0);

    Matrix<N1, N1> xdot;
    for (int i = 0; i < (2.0 / dt); i++) {
      observer.predict(u, dt);

      u = discB.solve(ref.minus(discA.times(ref)));

      xdot = plant.getA().times(observer.getXhat()).plus(plant.getB().times(u));

      time.add(i * dt);
      refData.add(ref.get(0, 0));
      xhat.add(observer.getXhat(0));
      udata.add(u.get(0, 0));
      xdotData.add(xdot.get(0, 0));
    }

    //    var chartBuilder = new XYChartBuilder();
    //    chartBuilder.title = "The Magic of Sensor Fusion";
    //    var chart = chartBuilder.build();

    //    chart.addSeries("Ref", time, refData);
    //    chart.addSeries("xHat", time, xhat);
    //    chart.addSeries("input", time, udata);
    ////    chart.addSeries("xdot", time, xdotData);

    //    new SwingWrapper<>(chart).displayChart();
    //    try {
    //      Thread.sleep(1000000000);
    //    } catch (InterruptedException e) {
    //    }

    assertEquals(ref.get(0, 0), observer.getXhat(0), 5);
  }

  @Test
  public void testUnscentedTransform() {
    // From FilterPy
    var ret = UnscentedKalmanFilter.unscentedTransform(Nat.N4(), Nat.N4(),
          Matrix.mat(Nat.N4(), Nat.N9()).fill(
              -0.9, -0.822540333075852, -0.8922540333075852, -0.9,
                  -0.9, -0.9774596669241481, -0.9077459666924148, -0.9, -0.9,
              1.0, 1.0, 1.077459666924148, 1.0, 1.0, 1.0, 0.9225403330758519, 1.0, 1.0,
              -0.9, -0.9, -0.9, -0.822540333075852, -0.8922540333075852, -0.9,
                  -0.9, -0.9774596669241481, -0.9077459666924148,
              1.0, 1.0, 1.0, 1.0, 1.077459666924148, 1.0, 1.0, 1.0, 0.9225403330758519
          ),
          VecBuilder.fill(
              -132.33333333,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667
          ),
          VecBuilder.fill(
              -129.34333333,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667,
              16.66666667
          ), (sigmas, weights) -> sigmas.times(Matrix.changeBoundsUnchecked(weights)), Matrix::minus
    );

    assertTrue(
          VecBuilder.fill(-0.9, 1, -0.9, 1).isEqual(
          ret.getFirst(), 1E-5
    ));

    assertTrue(
            Matrix.mat(Nat.N4(), Nat.N4()).fill(
                  2.02000002e-01, 2.00000500e-02, -2.69044710e-29,
                  -4.59511477e-29,
                  2.00000500e-02, 2.00001000e-01, -2.98781068e-29,
                  -5.12759588e-29,
                  -2.73372625e-29, -3.09882635e-29, 2.02000002e-01,
                  2.00000500e-02,
                  -4.67065917e-29, -5.10705197e-29, 2.00000500e-02,
                  2.00001000e-01
            ).isEqual(
            ret.getSecond(), 1E-5
    ));
  }
}
