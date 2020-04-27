/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import java.util.ArrayList;
import java.util.List;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Twist2d;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpiutil.math.VecBuilder;

import static org.junit.jupiter.api.Assertions.assertTrue;

class LTVUnicycleControllerTest {
  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  void testReachesReference() {
    final var controller = new LTVUnicycleController(
        VecBuilder.fill(0.1, 0.1, 0.15),
        VecBuilder.fill(8.8, 4.0),
        0.02
    );
    var robotPose = new Pose2d(2.7, 23.0, Rotation2d.fromDegrees(0.0));

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(2.75, 22.521, new Rotation2d(0)));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.846)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    controller.setTolerance(new Pose2d(0.01, 0.01, new Rotation2d(0.001)));

    List<Double> time = new ArrayList<>();

    List<Double> trajXs = new ArrayList<>();
    List<Double> trajYs = new ArrayList<>();
    List<Double> trajHeadings = new ArrayList<>();


    List<Double> robotXs = new ArrayList<>();
    List<Double> robotYs = new ArrayList<>();
    List<Double> robotHeadings = new ArrayList<>();

    final double kDt = 0.02;
    final var totalTime = trajectory.getTotalTimeSeconds();
    for (int i = 0; i < (totalTime / kDt); ++i) {
      var state = trajectory.sample(kDt * i);

      time.add(kDt * i);

      trajXs.add(state.poseMeters.getTranslation().getX());
      trajYs.add(state.poseMeters.getTranslation().getY());
      trajHeadings.add(state.poseMeters.getRotation().getRadians());

      var output = controller.calculate(robotPose, state);
      robotPose = robotPose.exp(new Twist2d(output.vxMetersPerSecond * kDt, 0,
          output.omegaRadiansPerSecond * kDt));

      robotXs.add(robotPose.getTranslation().getX());
      robotYs.add(robotPose.getTranslation().getY());
      robotHeadings.add(robotPose.getRotation().getRadians());
    }

    assertTrue(controller.atReference());

    // List<XYChart> charts = new ArrayList<XYChart>();

    // var chartBuilder = new XYChartBuilder();
    // chartBuilder.title = "LTV Unicycle Controller";
    // var chart = chartBuilder.build();

    // chart.addSeries("Trajectory", trajXs, trajYs);
    // chart.addSeries("Robot", robotXs, robotYs);
    // charts.add(chart);

    // var timeChartBuilder = new XYChartBuilder();
    // timeChartBuilder.title = "Time vs Position";
    // var timeChart = timeChartBuilder.build();

    // timeChart.addSeries("XRobot", time, robotXs);
    // timeChart.addSeries("YRobot", time, robotYs);
    // timeChart.addSeries("HeadingRobot", time, robotHeadings);
    // timeChart.addSeries("XTraj", time, trajXs);
    // timeChart.addSeries("YTraj", time, trajYs);
    // timeChart.addSeries("HeadingTraj", time, trajHeadings);
    // charts.add(timeChart);

    // new SwingWrapper<>(charts).displayChartMatrix();
    // try {
    //  Thread.sleep(1000000000);
    // } catch (InterruptedException e) {
    // }
  }
}
