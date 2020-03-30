/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.ArrayList;
import java.util.Collections;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVoltageConstraint;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DifferentialDriveVoltageConstraintTest {
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  @Test
  void testDifferentialDriveVoltageConstraint() {
    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);
    var kinematics = new DifferentialDriveKinematics(0.5);
    double maxVoltage = 10;
    var constraint = new DifferentialDriveVoltageConstraint(feedforward,
                                                            kinematics,
                                                            maxVoltage);

    Trajectory trajectory = TrajectoryGeneratorTest.getTrajectory(
        Collections.singletonList(constraint));

    var duration = trajectory.getTotalTimeSeconds();
    var t = 0.0;
    var dt = 0.02;

    while (t < duration) {
      var point = trajectory.sample(t);
      var chassisSpeeds = new ChassisSpeeds(
          point.velocityMetersPerSecond, 0,
          point.velocityMetersPerSecond * point.curvatureRadPerMeter
      );

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      t += dt;

      // Not really a strictly-correct test as we're using the chassis accel instead of the
      // wheel accel, but much easier than doing it "properly" and a reasonable check anyway
      assertAll(
          () -> assertTrue(feedforward.calculate(wheelSpeeds.leftMetersPerSecond,
                                                 point.accelerationMetersPerSecondSq)
                               <= maxVoltage + 0.05),
          () -> assertTrue(feedforward.calculate(wheelSpeeds.leftMetersPerSecond,
                                                 point.accelerationMetersPerSecondSq)
                               >= -maxVoltage - 0.05),
          () -> assertTrue(feedforward.calculate(wheelSpeeds.rightMetersPerSecond,
                                                 point.accelerationMetersPerSecondSq)
                               <= maxVoltage + 0.05),
          () -> assertTrue(feedforward.calculate(wheelSpeeds.rightMetersPerSecond,
                                                 point.accelerationMetersPerSecondSq)
                               >= -maxVoltage - 0.05)
      );
    }
  }

  @Test
  void testEndpointHighCurvature() {
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);

    // Large trackwidth - need to test with radius of curvature less than half of trackwidth
    var kinematics = new DifferentialDriveKinematics(3);
    double maxVoltage = 10;
    var constraint = new DifferentialDriveVoltageConstraint(feedforward,
                                                            kinematics,
                                                            maxVoltage);

    var config = new TrajectoryConfig(12, 12).addConstraint(constraint);

    // Radius of curvature should be ~1 meter.
    assertDoesNotThrow(() -> TrajectoryGenerator.generateTrajectory(
        new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
        new ArrayList<Translation2d>(),
        new Pose2d(0, 1, Rotation2d.fromDegrees(180)),
        config));

    assertDoesNotThrow(() -> TrajectoryGenerator.generateTrajectory(
        new Pose2d(0, 1, Rotation2d.fromDegrees(180)),
        new ArrayList<Translation2d>(),
        new Pose2d(1, 0, Rotation2d.fromDegrees(90)),
        config.setReversed(true)));

  }
}
