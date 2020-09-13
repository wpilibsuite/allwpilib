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
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.trajectory.Trajectory;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpiutil.math.MathUtil;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

class HolonomicDriveControllerTest {
  private static final double kTolerance = 1 / 12.0;
  private static final double kAngularTolerance = Math.toRadians(2);

  @Test
  @SuppressWarnings("PMD.AvoidInstantiatingObjectsInLoops")
  void testReachesReference() {
    HolonomicDriveController controller = new HolonomicDriveController(
        new PIDController(1.0, 0.0, 0.0),
        new PIDController(1.0, 0.0, 0.0),
        new ProfiledPIDController(1.0, 0.0, 0.0,
            new TrapezoidProfile.Constraints(6.28, 3.14))
    );
    Pose2d robotPose = new Pose2d(2.7, 23.0, Rotation2d.fromDegrees(0.0));

    List<Pose2d> waypoints = new ArrayList<>();
    waypoints.add(new Pose2d(2.75, 22.521, new Rotation2d(0)));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.8)));

    TrajectoryConfig config = new TrajectoryConfig(8.0, 4.0);
    Trajectory trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    final double kDt = 0.02;
    final double kTotalTime = trajectory.getTotalTimeSeconds();

    for (int i = 0; i < (kTotalTime / kDt); i++) {
      Trajectory.State state = trajectory.sample(kDt * i);
      ChassisSpeeds output = controller.calculate(robotPose, state, new Rotation2d());

      robotPose = robotPose.exp(new Twist2d(output.vxMetersPerSecond * kDt,
          output.vyMetersPerSecond * kDt, output.omegaRadiansPerSecond * kDt));
    }

    final List<Trajectory.State> states = trajectory.getStates();
    final Pose2d endPose = states.get(states.size() - 1).poseMeters;

    // Java lambdas require local variables referenced from a lambda expression
    // must be final or effectively final.
    final Pose2d finalRobotPose = robotPose;

    assertAll(
        () -> assertEquals(endPose.getX(), finalRobotPose.getX(),
            kTolerance),
        () -> assertEquals(endPose.getY(), finalRobotPose.getY(),
            kTolerance),
        () -> assertEquals(0.0,
            MathUtil.normalizeAngle(finalRobotPose.getRotation().getRadians()),
            kAngularTolerance)
    );
  }
}
