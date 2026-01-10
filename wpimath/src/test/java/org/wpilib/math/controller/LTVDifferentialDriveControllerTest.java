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
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.numbers.N5;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.NumericalIntegration;
import org.wpilib.math.system.plant.LinearSystemId;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.trajectory.TrajectoryGenerator;
import org.wpilib.math.util.MathUtil;
import org.wpilib.math.util.Nat;

class LTVDifferentialDriveControllerTest {
  private static final double kTolerance = 1 / 12.0;
  private static final double kAngularTolerance = Math.toRadians(2);

  /** States of the drivetrain system. */
  static class State {
    /// X position in global coordinate frame.
    public static final int kX = 0;

    /// Y position in global coordinate frame.
    public static final int kY = 1;

    /// Heading in global coordinate frame.
    public static final int kHeading = 2;

    /// Left encoder velocity.
    public static final int kLeftVelocity = 3;

    /// Right encoder velocity.
    public static final int kRightVelocity = 4;
  }

  private static final double kLinearV = 3.02; // V/(m/s)
  private static final double kLinearA = 0.642; // V/(m/s²)
  private static final double kAngularV = 1.382; // V/(m/s)
  private static final double kAngularA = 0.08495; // V/(m/s²)
  private static final LinearSystem<N2, N2, N2> plant =
      LinearSystemId.identifyDrivetrainSystem(kLinearV, kLinearA, kAngularV, kAngularA);
  private static final double kTrackwidth = 0.9;

  private static Matrix<N5, N1> dynamics(Matrix<N5, N1> x, Matrix<N2, N1> u) {
    double v = (x.get(State.kLeftVelocity, 0) + x.get(State.kRightVelocity, 0)) / 2.0;

    var xdot = new Matrix<>(Nat.N5(), Nat.N1());
    xdot.set(0, 0, v * Math.cos(x.get(State.kHeading, 0)));
    xdot.set(1, 0, v * Math.sin(x.get(State.kHeading, 0)));
    xdot.set(2, 0, (x.get(State.kRightVelocity, 0) - x.get(State.kLeftVelocity, 0)) / kTrackwidth);
    xdot.assignBlock(
        3, 0, plant.getA().times(x.block(Nat.N2(), Nat.N1(), 3, 0)).plus(plant.getB().times(u)));
    return xdot;
  }

  @Test
  void testReachesReference() {
    final double kDt = 0.02;

    final var controller =
        new LTVDifferentialDriveController(
            plant,
            kTrackwidth,
            VecBuilder.fill(0.0625, 0.125, 2.5, 0.95, 0.95),
            VecBuilder.fill(12.0, 12.0),
            kDt);
    var robotPose = new Pose2d(2.7, 23.0, Rotation2d.kZero);

    final var waypoints = new ArrayList<Pose2d>();
    waypoints.add(new Pose2d(2.75, 22.521, Rotation2d.kZero));
    waypoints.add(new Pose2d(24.73, 19.68, new Rotation2d(5.846)));
    var config = new TrajectoryConfig(8.8, 0.1);
    final var trajectory = TrajectoryGenerator.generateTrajectory(waypoints, config);

    var x =
        MatBuilder.fill(
            Nat.N5(),
            Nat.N1(),
            robotPose.getX(),
            robotPose.getY(),
            robotPose.getRotation().getRadians(),
            0.0,
            0.0);

    final var totalTime = trajectory.duration;
    for (int i = 0; i < (totalTime / kDt); ++i) {
      var state = trajectory.sampleAt(kDt * i);
      robotPose =
          new Pose2d(
              x.get(State.kX, 0), x.get(State.kY, 0), new Rotation2d(x.get(State.kHeading, 0)));
      final var output =
          controller.calculate(
              robotPose,
              x.get(State.kLeftVelocity, 0),
              x.get(State.kRightVelocity, 0),
              new DifferentialSample(state, new DifferentialDriveKinematics(kTrackwidth)));

      x =
          NumericalIntegration.rkdp(
              LTVDifferentialDriveControllerTest::dynamics,
              x,
              VecBuilder.fill(output.left, output.right),
              kDt);
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
