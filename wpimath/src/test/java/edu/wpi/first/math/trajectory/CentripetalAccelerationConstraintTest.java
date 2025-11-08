// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.trajectory.constraint.CentripetalAccelerationConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class CentripetalAccelerationConstraintTest {
  @Test
  void testCentripetalAccelerationConstraint() {
    double maxCentripetalAcceleration = Units.feetToMeters(7.0); // 7 feet per second squared
    var constraint = new CentripetalAccelerationConstraint(maxCentripetalAcceleration);

    Trajectory<SplineSample> trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(new Pose2d(0, 0, Rotation2d.kZero), new Pose2d(1, 0, Rotation2d.kZero)),
            new TrajectoryConfig(1, 1).addConstraint(constraint));
    var duration = trajectory.duration.in(Seconds);

    for (double t = 0; t < duration; t += 0.02) {
      var point = trajectory.sampleAt(t);
      var centripetalAcceleration = Math.pow(point.velocity.vx, 2) * point.curvature;

      assertTrue(centripetalAcceleration <= maxCentripetalAcceleration + 0.05);
    }
  }
}
