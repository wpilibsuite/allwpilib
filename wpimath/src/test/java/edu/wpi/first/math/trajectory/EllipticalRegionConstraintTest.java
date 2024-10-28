// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Ellipse2d;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.trajectory.constraint.EllipticalRegionConstraint;
import edu.wpi.first.math.trajectory.constraint.MaxVelocityConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class EllipticalRegionConstraintTest {
  @Test
  void testConstraint() {
    double maxVelocity = Units.feetToMeters(3.0);
    var ellipse =
        new Ellipse2d(
            new Pose2d(Units.feetToMeters(5.0), Units.feetToMeters(2.5), Rotation2d.kPi),
            Units.feetToMeters(5.0),
            Units.feetToMeters(2.5));

    var trajectory =
        TrajectoryGeneratorTest.getTrajectory(
            List.of(
                new EllipticalRegionConstraint(ellipse, new MaxVelocityConstraint(maxVelocity))));

    boolean exceededConstraintOutsideRegion = false;
    for (var point : trajectory.getStates()) {
      if (ellipse.contains(point.poseMeters.getTranslation())) {
        assertTrue(Math.abs(point.velocityMetersPerSecond) < maxVelocity + 0.05);
      } else if (Math.abs(point.velocityMetersPerSecond) >= maxVelocity + 0.05) {
        exceededConstraintOutsideRegion = true;
      }
    }
    assertTrue(exceededConstraintOutsideRegion);
  }
}
