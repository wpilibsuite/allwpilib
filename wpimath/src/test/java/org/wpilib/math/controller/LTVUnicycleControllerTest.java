// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.ArrayList;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Twist2d;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.trajectory.TrajectoryGenerator;
import org.wpilib.math.util.MathUtil;

class LTVUnicycleControllerTest {
  private static final double kTolerance = 1 / 12.0;
  private static final double kAngularTolerance = Math.toRadians(2);

  @Test
  void testReachesReference() {
    final double kDt = 0.02;

    final var controller =
        new LTVUnicycleController(
            VecBuilder.fill(0.0625, 0.125, 2.5), VecBuilder.fill(4.0, 4.0), kDt);
    var robotPose = new Pose2d(2.7, 23.0, Rotation2d.kZero);

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(2.75, 22.521, Rotation2d.kZero));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.846)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final var totalTime = trajectory.duration;
    for (int i = 0; i < (totalTime / kDt); ++i) {
      var state = trajectory.sampleAt(kDt * i);

      var output = controller.calculate(robotPose, state);
      robotPose = robotPose.plus(new Twist2d(output.vx * kDt, 0, output.omega * kDt).exp());
    }

    final var states = trajectory.getSamples();
    final var endPose = states.getLast().pose;

    // Java lambdas require local variables referenced from a lambda expression
    // must be final or effectively final.
    final var finalRobotPose = robotPose;
    assertAll(
        () -> assertEquals(endPose.getX(), finalRobotPose.getX(), kTolerance),
        () -> assertEquals(endPose.getY(), finalRobotPose.getY(), kTolerance),
        () ->
            assertEquals(
                0.0,
                MathUtil.angleModulus(
                    endPose.getRotation().getRadians() - finalRobotPose.getRotation().getRadians()),
                kAngularTolerance));
  }
}
