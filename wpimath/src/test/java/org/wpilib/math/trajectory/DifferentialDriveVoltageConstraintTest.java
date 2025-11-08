// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.constraint.DifferentialDriveVoltageConstraint;
import java.util.ArrayList;
import java.util.Collections;
import org.junit.jupiter.api.Test;

class DifferentialDriveVoltageConstraintTest {
  @Test
  void testDifferentialDriveVoltageConstraint() {
    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);
    var kinematics = new DifferentialDriveKinematics(0.5);
    double maxVoltage = 10;
    var constraint = new DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage);

    Trajectory trajectory =
        TrajectoryGeneratorTest.getTrajectory(Collections.singletonList(constraint));

    var duration = trajectory.getTotalTime();
    var t = 0.0;
    var dt = 0.02;

    while (t < duration) {
      var point = trajectory.sample(t);
      var chassisSpeeds = new ChassisSpeeds(point.velocity, 0, point.velocity * point.curvature);
      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      t += dt;
      var acceleration = point.acceleration;

      // Not really a strictly-correct test as we're using the chassis accel instead of the
      // wheel accel, but much easier than doing it "properly" and a reasonable check anyway
      assertAll(
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.left, wheelSpeeds.left + dt * acceleration)
                      <= maxVoltage + 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.left, wheelSpeeds.left + dt * acceleration)
                      >= -maxVoltage - 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.right, wheelSpeeds.right + dt * acceleration)
                      <= maxVoltage + 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.right, wheelSpeeds.right + dt * acceleration)
                      >= -maxVoltage - 0.05));
    }
  }

  @Test
  void testEndpointHighCurvature() {
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);

    // Large trackwidth - need to test with radius of curvature less than half of trackwidth
    var kinematics = new DifferentialDriveKinematics(3);
    double maxVoltage = 10;
    var constraint = new DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage);

    var config = new TrajectoryConfig(12, 12).addConstraint(constraint);

    // Radius of curvature should be ~1 meter.
    assertDoesNotThrow(
        () ->
            TrajectoryGenerator.generateTrajectory(
                new Pose2d(1, 0, Rotation2d.kCCW_Pi_2),
                new ArrayList<>(),
                new Pose2d(0, 1, Rotation2d.kPi),
                config));

    assertDoesNotThrow(
        () ->
            TrajectoryGenerator.generateTrajectory(
                new Pose2d(0, 1, Rotation2d.kPi),
                new ArrayList<>(),
                new Pose2d(1, 0, Rotation2d.kCCW_Pi_2),
                config.setReversed(true)));
  }
}
