// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Seconds;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.constraint.DifferentialDriveVoltageConstraint;

class DifferentialDriveVoltageConstraintTest {
  @Test
  void testDifferentialDriveVoltageConstraint() {
    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);
    var kinematics = new DifferentialDriveKinematics(0.5);
    double maxVoltage = 10;
    var constraint = new DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage);

    Trajectory<SplineSample> trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(new Pose2d(0, 0, Rotation2d.kZero), new Pose2d(1, 0, Rotation2d.kZero)),
            new TrajectoryConfig(1, 1).addConstraint(constraint));
    var duration = trajectory.duration;

    for (double t = 0; t < duration; t += 0.02) {
      var point = trajectory.sampleAt(t);
      var chassisSpeeds =
          new ChassisSpeeds(point.velocity.vx, 0, point.velocity.vx * point.curvature);
      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      var acceleration = point.acceleration.ax;

      // Not really a strictly-correct test as we're using the chassis accel instead of the
      // wheel accel, but much easier than doing it "properly" and a reasonable check anyway
      assertAll(
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.left, wheelSpeeds.left + 0.02 * acceleration)
                      <= maxVoltage + 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.left, wheelSpeeds.left + 0.02 * acceleration)
                      >= -maxVoltage - 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.right, wheelSpeeds.right + 0.02 * acceleration)
                      <= maxVoltage + 0.05),
          () ->
              assertTrue(
                  feedforward.calculate(wheelSpeeds.right, wheelSpeeds.right + 0.02 * acceleration)
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
