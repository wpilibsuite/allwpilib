// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import java.util.ArrayList;
import org.junit.jupiter.api.Test;

class RamseteControllerTest {
  private static final double kTolerance = 1 / 12.0;
  private static final double kAngularTolerance = Math.toRadians(2);

  @SuppressWarnings("removal")
  @Test
  void testReachesReference() {
    final var controller = new RamseteController(2.0, 0.7);
    var robotPose = new Pose2d(2.7, 23.0, Rotation2d.kZero);

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(2.75, 22.521, Rotation2d.kZero));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.846)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final double kDt = 0.02;
    final var totalTime = trajectory.getTotalTimeSeconds();
    for (int i = 0; i < (totalTime / kDt); ++i) {
      var state = trajectory.sample(kDt * i);

      var output = controller.calculate(robotPose, state);
      robotPose =
          robotPose.exp(
              new Twist2d(output.vxMetersPerSecond * kDt, 0, output.omegaRadiansPerSecond * kDt));
    }

    final var states = trajectory.getStates();
    final var endPose = states.get(states.size() - 1).poseMeters;

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
