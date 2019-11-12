/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.Collections;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.kinematics.ChassisSpeeds;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveVoltageConstraint;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DifferentialDriveVoltageConstraintTest {
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  @Test
  void testDifferentialDriveVoltageConstraint() {
    // Pick an unreasonably large kA to ensure the constraint has to do some work
    var feedforward = new SimpleMotorFeedforward(1, 1, 3);
    var kinematics = new DifferentialDriveKinematics(.5);
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
}
