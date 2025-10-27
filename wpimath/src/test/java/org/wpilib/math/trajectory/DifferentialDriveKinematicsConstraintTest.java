// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.math.kinematics.ChassisSpeeds;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import org.wpilib.math.util.Units;
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

    var duration = trajectory.getTotalTime();
    var t = 0.0;
    var dt = 0.02;

    while (t < duration) {
      var point = trajectory.sample(t);
      var chassisSpeeds = new ChassisSpeeds(point.velocity, 0, point.velocity * point.curvature);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      t += dt;
      assertAll(
          () -> assertTrue(wheelSpeeds.left <= maxVelocity + 0.05),
          () -> assertTrue(wheelSpeeds.right <= maxVelocity + 0.05));
    }
  }
}
