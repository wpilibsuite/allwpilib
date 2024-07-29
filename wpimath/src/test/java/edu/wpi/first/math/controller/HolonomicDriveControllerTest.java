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
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.trajectory.Trajectory;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

class HolonomicDriveControllerTest {
  private static final double kTolerance = 1 / 12.0;
  private static final double kAngularTolerance = Math.toRadians(2);

  @Test
  void testReachesReference() {
    HolonomicDriveController controller =
        new HolonomicDriveController(
            new PIDController(1.0, 0.0, 0.0),
            new PIDController(1.0, 0.0, 0.0),
            new ProfiledPIDController(
                1.0, 0.0, 0.0, new TrapezoidProfile.Constraints(2.0 * Math.PI, Math.PI)));
    Pose2d robotPose = new Pose2d(2.7, 23.0, Rotation2d.kZero);

    List<Pose2d> waypoints = new ArrayList<>();
    waypoints.add(new Pose2d(2.75, 22.521, Rotation2d.kZero));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.8)));

    TrajectoryConfig config = new TrajectoryConfig(8.0, 4.0);
    Trajectory trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final double kDt = 0.02;
    final double kTotalTime = trajectory.getTotalTimeSeconds();

    for (int i = 0; i < (kTotalTime / kDt); i++) {
      Trajectory.State state = trajectory.sample(kDt * i);
      ChassisSpeeds output = controller.calculate(robotPose, state, Rotation2d.kZero);

      robotPose =
          robotPose.exp(
              new Twist2d(
                  output.vxMetersPerSecond * kDt,
                  output.vyMetersPerSecond * kDt,
                  output.omegaRadiansPerSecond * kDt));
    }

    final List<Trajectory.State> states = trajectory.getStates();
    final Pose2d endPose = states.get(states.size() - 1).poseMeters;

    // Java lambdas require local variables referenced from a lambda expression
    // must be final or effectively final.
    final Pose2d finalRobotPose = robotPose;

    assertAll(
        () -> assertEquals(endPose.getX(), finalRobotPose.getX(), kTolerance),
        () -> assertEquals(endPose.getY(), finalRobotPose.getY(), kTolerance),
        () ->
            assertEquals(
                0.0,
                MathUtil.angleModulus(finalRobotPose.getRotation().getRadians()),
                kAngularTolerance));
  }

  @Test
  void testDoesNotRotateUnnecessarily() {
    var controller =
        new HolonomicDriveController(
            new PIDController(1, 0, 0),
            new PIDController(1, 0, 0),
            new ProfiledPIDController(1, 0, 0, new TrapezoidProfile.Constraints(4, 2)));

    ChassisSpeeds speeds =
        controller.calculate(
            new Pose2d(0, 0, new Rotation2d(1.57)), Pose2d.kZero, 0, new Rotation2d(1.57));

    assertEquals(0.0, speeds.omegaRadiansPerSecond);
  }
}
