// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.math.util.Units;
import java.util.Collections;
import org.junit.jupiter.api.Test;

class DifferentialDriveKinematicsConstraintTest {
  @Test
  void testDifferentialDriveKinematicsConstraint() {
    double maxVelocity = Units.feetToMeters(12.0); // 12 feet per second
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(27));
    var constraint = new DifferentialDriveKinematicsConstraint(kinematics, maxVelocity);

    Trajectory trajectory =
        TrajectoryGeneratorTest.getTrajectory(Collections.singletonList(constraint));

    var duration = trajectory.getTotalTimeSeconds();
    var t = 0.0;
    var dt = 0.02;

    while (t < duration) {
      var point = trajectory.sample(t);
      var chassisSpeeds =
          new ChassisSpeeds(
              point.velocityMetersPerSecond,
              0,
              point.velocityMetersPerSecond * point.curvatureRadPerMeter);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      t += dt;
      assertAll(
          () -> assertTrue(wheelSpeeds.leftMetersPerSecond <= maxVelocity + 0.05),
          () -> assertTrue(wheelSpeeds.rightMetersPerSecond <= maxVelocity + 0.05));
    }
  }
}
