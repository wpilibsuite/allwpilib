// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.ChassisSpeeds;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class DifferentialDriveKinematicsConstraintTest {
  @Test
  void testDifferentialDriveKinematicsConstraint() {
    double maxVelocity = Units.feetToMeters(12.0); // 12 feet per second
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(27));
    var constraint = new DifferentialDriveKinematicsConstraint(kinematics, maxVelocity);

    Trajectory<SplineSample> trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(new Pose2d(0, 0, Rotation2d.kZero), new Pose2d(1, 0, Rotation2d.kZero)),
            new TrajectoryConfig(1, 1).addConstraint(constraint));
    var duration = trajectory.duration.in(Seconds);

    for (double t = 0; t < duration; t += 0.02) {
      var point = trajectory.sampleAt(t);
      var chassisSpeeds = new ChassisSpeeds(point.vel.vx, 0, point.vel.vx * point.curvature);

      var wheelSpeeds = kinematics.toWheelSpeeds(chassisSpeeds);

      assertAll(
          () -> assertTrue(wheelSpeeds.left <= maxVelocity + 0.05),
          () -> assertTrue(wheelSpeeds.right <= maxVelocity + 0.05));
    }
  }
}
