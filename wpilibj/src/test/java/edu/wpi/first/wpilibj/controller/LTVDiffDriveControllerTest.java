/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import java.util.ArrayList;
import java.util.List;

import org.ejml.dense.row.CommonOps_DDRM;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.estimator.DifferentialDriveStateEstimator;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVelocitySystemConstraint;
import edu.wpi.first.wpiutil.math.MatBuilder;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N10;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N5;

import static org.junit.jupiter.api.Assertions.assertTrue;

@SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops",
    "MemberName", "PMD.SingularField", "PMD.ExcessiveMethodLength"})
class LTVDiffDriveControllerTest {
  private Matrix<N10, N1> trueXhat;
  private Matrix<N2, N1> u;

  private LinearSystem<N2, N2, N2> plant;

  private LTVDiffDriveController controller;
  private DifferentialDriveStateEstimator estimator;
  private ControlAffinePlantInversionFeedforward<N10, N2> feedforward;

  private DifferentialDriveKinematics kinematics;

  private Trajectory trajectory;

  private double totalTime;

  @BeforeEach
  void setUp() {
    final double kDt = 0.02;

    plant = LinearSystemId.identifyDrivetrainSystem(
            3.02, 0.642, 1.382, 0.08495);

    kinematics = new DifferentialDriveKinematics(1);

    controller = new LTVDiffDriveController(
            plant,
            VecBuilder.fill(0.0625, 0.125, 2.5, 0.95, 0.95),
            VecBuilder.fill(12.0, 12.0),
            kinematics,
            kDt);

    feedforward = new ControlAffinePlantInversionFeedforward<>(
        Nat.N10(), Nat.N2(), controller::getDynamics, kDt);

    estimator = new DifferentialDriveStateEstimator(
      plant,
      MatrixUtils.zeros(Nat.N10()),
      new MatBuilder<>(Nat.N10(), Nat.N1()).fill(
        0.002, 0.002, 0.0001, 1.5, 1.5, 0.5, 0.5, 10.0, 10.0, 2.0),
      new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.005, 0.005),
      new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.5, 0.5, 0.5),
      kinematics,
      kDt);

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d());
    waypoints.add(new Pose2d(4.8768, 2.7432, new Rotation2d()));

    var config = new TrajectoryConfig(12 / 3.02, (12 / 0.642) - 16.5);

    var constraint = new DifferentialDriveVelocitySystemConstraint(plant, kinematics, 10);
    config.addConstraint(constraint);

    trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);
    totalTime = trajectory.getTotalTimeSeconds();
  }

  @SuppressWarnings("ParameterName")
  private Matrix<N2, N1> scaleCappedU(Matrix<N2, N1> u) {
    boolean isOutputCapped = Math.abs(u.get(0, 0)) > 12.0 || Math.abs(u.get(1, 0)) > 12.0;

    if (isOutputCapped) {
      u = u.times(12.0 / CommonOps_DDRM.elementMaxAbs(u.getStorage().getDDRM()));
    }
    return u;
  }

  @Test
  void trackingTest() {
    controller.reset();
    estimator.reset();
    controller.setTolerance(new Pose2d(0.06, 0.06, new Rotation2d(0.1)), 0.5);

    final double kDt = 0.02;

    List<Double> time = new ArrayList<>();

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> trajHeading = new ArrayList<>();
    List<Double> trajLeftVel = new ArrayList<>();
    List<Double> trajRightVel = new ArrayList<>();

    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> observerHeading = new ArrayList<>();
    List<Double> observerLeftVel = new ArrayList<>();
    List<Double> observerRightVel = new ArrayList<>();

    trueXhat = MatrixUtils.zeros(Nat.N10(), Nat.N1());

    u = MatrixUtils.zeros(Nat.N2(), Nat.N1());

    var prevStateRef = MatrixUtils.zeros(Nat.N5());

    double t = 0.0;

    while (t <= totalTime) {
      var y = estimator.getLocalMeasurementModel(
          trueXhat, MatrixUtils.zeros(Nat.N2(), Nat.N1()));

      var currentState = estimator.updateWithTime(
          y.get(0, 0), y.get(1, 0), y.get(2, 0), u, t);

      var desiredState = trajectory.sample(t);

      var wheelVelocities = kinematics.toWheelSpeeds(
          new ChassisSpeeds(desiredState.velocityMetersPerSecond,
                0,
                desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter));

      @SuppressWarnings("VariableDeclarationUsageDistance")
      Matrix<N5, N1> stateRef = new MatBuilder<>(Nat.N5(), Nat.N1()).fill(
              desiredState.poseMeters.getTranslation().getX(),
              desiredState.poseMeters.getTranslation().getY(),
              desiredState.poseMeters.getRotation().getRadians(),
              wheelVelocities.leftMetersPerSecond,
              wheelVelocities.rightMetersPerSecond);

      /* Logging for Graphing */
      time.add(t);

      observerXs.add(currentState.get(0, 0));
      observerYs.add(currentState.get(1, 0));
      observerHeading.add(currentState.get(2, 0));
      observerLeftVel.add(currentState.get(3, 0));
      observerRightVel.add(currentState.get(4, 0));

      trajXs.add(prevStateRef.get(0, 0));
      trajYs.add(prevStateRef.get(1, 0));
      trajHeading.add(prevStateRef.get(2, 0));
      trajLeftVel.add(prevStateRef.get(3, 0));
      trajRightVel.add(prevStateRef.get(4, 0));

      prevStateRef = stateRef;

      var augmentedRef = MatrixUtils.zeros(Nat.N10());
      augmentedRef.getStorage().insertIntoThis(0, 0, stateRef.getStorage());

      u = controller.calculate(
          currentState.block(Nat.N5(), Nat.N1(), 0, 0), desiredState)
          .plus(feedforward.calculate(augmentedRef, feedforward.getR()));

      u = scaleCappedU(u);

      t += kDt;

      trueXhat = RungeKutta.rungeKutta(controller::getDynamics, trueXhat, u, kDt);
      assertTrue(controller.atReference());
    }

    /*
    List<XYChart> charts = new ArrayList<XYChart>();

    var chartBuilder = new XYChartBuilder();
    chartBuilder.title = "The Magic of Sensor Fusion";
    var chart = chartBuilder.build();

    chart.addSeries("Trajectory", trajXs, trajYs);
    chart.addSeries("xHat", observerXs, observerYs);
    charts.add(chart);

    var chartBuilderHeading = new XYChartBuilder();
    chartBuilderHeading.title = "Heading versus Time";
    var chartHeading = chartBuilderHeading.build();

    chartHeading.addSeries("Trajectory", time, trajHeading);
    chartHeading.addSeries("xHat", time, observerHeading);
    charts.add(chartHeading);

    var chartBuilderLeftVelocity = new XYChartBuilder();
    chartBuilderLeftVelocity.title = "Left Velocity versus Time";
    var chartLeftVelocity = chartBuilderLeftVelocity.build();

    chartLeftVelocity.addSeries("Trajectory", time, trajLeftVel);
    chartLeftVelocity.addSeries("xHat", time, observerLeftVel);
    charts.add(chartLeftVelocity);

    var chartBuilderRightVelocity = new XYChartBuilder();
    chartBuilderRightVelocity.title = "Right Velocity versus Time";
    var chartRightVelocity = chartBuilderRightVelocity.build();

    chartRightVelocity.addSeries("Trajectory", time, trajRightVel);
    chartRightVelocity.addSeries("xHat", time, observerRightVel);

    charts.add(chartRightVelocity);

    new SwingWrapper<>(charts).displayChartMatrix();
    try {
     Thread.sleep(1000000000);
    } catch (InterruptedException e) {
    }
    */
  }

  @Test
  void trackingTestNoise() {
    controller.reset();
    estimator.reset();
    controller.setTolerance(new Pose2d(0.06, 0.06, new Rotation2d(0.1)), 0.5);

    double kDt = 0.02;

    List<Double> time = new ArrayList<>();

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> trajHeading = new ArrayList<>();
    List<Double> trajLeftVel = new ArrayList<>();
    List<Double> trajRightVel = new ArrayList<>();

    List<Double> observerXs = new ArrayList<>();
    List<Double> observerYs = new ArrayList<>();
    List<Double> observerHeading = new ArrayList<>();
    List<Double> observerLeftVel = new ArrayList<>();
    List<Double> observerRightVel = new ArrayList<>();

    trueXhat = MatrixUtils.zeros(Nat.N10(), Nat.N1());

    u = MatrixUtils.zeros(Nat.N2(), Nat.N1());

    var prevStateRef = MatrixUtils.zeros(Nat.N5());

    double t = 0.0;

    while (t <= totalTime) {
      kDt = 0.02 + StateSpaceUtil.makeWhiteNoiseVector(
            new MatBuilder<>(Nat.N1(), Nat.N1()).fill(0.0005)).get(0, 0);

      var y = estimator.getLocalMeasurementModel(trueXhat, MatrixUtils.zeros(Nat.N2(), Nat.N1()));

      y = y.plus(StateSpaceUtil.makeWhiteNoiseVector(
            new MatBuilder<>(Nat.N3(), Nat.N1()).fill(0.0001, 0.005, 0.005)));

      var currentState = estimator.updateWithTime(
          y.get(0, 0), y.get(1, 0), y.get(2, 0), u, t);

      var desiredState = trajectory.sample(t);

      var wheelVelocities = kinematics.toWheelSpeeds(
          new ChassisSpeeds(desiredState.velocityMetersPerSecond,
                0,
                desiredState.velocityMetersPerSecond * desiredState.curvatureRadPerMeter));

      @SuppressWarnings("VariableDeclarationUsageDistance")
      Matrix<N5, N1> stateRef = new MatBuilder<>(Nat.N5(), Nat.N1()).fill(
              desiredState.poseMeters.getTranslation().getX(),
              desiredState.poseMeters.getTranslation().getY(),
              desiredState.poseMeters.getRotation().getRadians(),
              wheelVelocities.leftMetersPerSecond,
              wheelVelocities.rightMetersPerSecond);

      time.add(t);

      observerXs.add(currentState.get(0, 0));
      observerYs.add(currentState.get(1, 0));
      observerHeading.add(currentState.get(2, 0));
      observerLeftVel.add(currentState.get(3, 0));
      observerRightVel.add(currentState.get(4, 0));

      trajXs.add(prevStateRef.get(0, 0));
      trajYs.add(prevStateRef.get(1, 0));
      trajHeading.add(prevStateRef.get(2, 0));
      trajLeftVel.add(prevStateRef.get(3, 0));
      trajRightVel.add(prevStateRef.get(4, 0));

      prevStateRef = stateRef;

      var augmentedRef = MatrixUtils.zeros(Nat.N10());
      augmentedRef.getStorage().insertIntoThis(0, 0, stateRef.getStorage());

      u = controller.calculate(
          currentState.block(Nat.N5(), Nat.N1(), 0, 0), desiredState)
          .plus(feedforward.calculate(augmentedRef, feedforward.getR()));

      u = scaleCappedU(u);

      t += kDt;

      trueXhat = RungeKutta.rungeKutta(controller::getDynamics, trueXhat, u, kDt);
      assertTrue(controller.atReference());
    }

    /*
    List<XYChart> charts = new ArrayList<XYChart>();

    var chartBuilder = new XYChartBuilder();
    chartBuilder.title = "The Magic of Sensor Fusion";
    var chart = chartBuilder.build();

    chart.addSeries("Trajectory", trajXs, trajYs);
    chart.addSeries("xHat", observerXs, observerYs);
    charts.add(chart);

    var chartBuilderHeading = new XYChartBuilder();
    chartBuilderHeading.title = "Heading versus Time";
    var chartHeading = chartBuilderHeading.build();

    chartHeading.addSeries("Trajectory", time, trajHeading);
    chartHeading.addSeries("xHat", time, observerHeading);
    charts.add(chartHeading);

    var chartBuilderLeftVelocity = new XYChartBuilder();
    chartBuilderLeftVelocity.title = "Left Velocity versus Time";
    var chartLeftVelocity = chartBuilderLeftVelocity.build();

    chartLeftVelocity.addSeries("Trajectory", time, trajLeftVel);
    chartLeftVelocity.addSeries("xHat", time, observerLeftVel);
    charts.add(chartLeftVelocity);

    var chartBuilderRightVelocity = new XYChartBuilder();
    chartBuilderRightVelocity.title = "Right Velocity versus Time";
    var chartRightVelocity = chartBuilderRightVelocity.build();

    chartRightVelocity.addSeries("Trajectory", time, trajRightVel);
    chartRightVelocity.addSeries("xHat", time, observerRightVel);

    charts.add(chartRightVelocity);

    new SwingWrapper<>(charts).displayChartMatrix();
    try {
     Thread.sleep(1000000000);
    } catch (InterruptedException e) {
    }
    */
  }
}
