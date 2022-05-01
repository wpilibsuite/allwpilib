// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.trajectory.constraint.MaxVelocityConstraint;
import edu.wpi.first.math.trajectory.constraint.RectangularRegionConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class RectangularRegionConstraintTest {
  @Test
  void testConstraint() {
    // Create constraints
    double maxVelocity = Units.feetToMeters(3.0);
    var maxVelocityConstraint = new MaxVelocityConstraint(maxVelocity);
    var regionConstraint =
        new RectangularRegionConstraint(
            new Translation2d(Units.feetToMeters(1.0), Units.feetToMeters(1.0)),
            new Translation2d(Units.feetToMeters(7.0), Units.feetToMeters(27.0)),
            maxVelocityConstraint);

    // Get trajectory
    var trajectory = TrajectoryGeneratorTest.getTrajectory(List.of(regionConstraint));

    // Iterate through trajectory and check constraints
    boolean exceededConstraintOutsideRegion = false;
    for (var point : trajectory.getStates()) {
      if (regionConstraint.isPoseInRegion(point.poseMeters)) {
        assertTrue(Math.abs(point.velocityMetersPerSecond) < maxVelocity + 0.05);
      } else if (Math.abs(point.velocityMetersPerSecond) >= maxVelocity + 0.05) {
        exceededConstraintOutsideRegion = true;
      }
    }
    assertTrue(exceededConstraintOutsideRegion);
  }

  @Test
  void testIsPoseWithinRegion() {
    double maxVelocity = Units.feetToMeters(3.0);
    var maxVelocityConstraint = new MaxVelocityConstraint(maxVelocity);
    var regionConstraint =
        new RectangularRegionConstraint(
            new Translation2d(Units.feetToMeters(1.0), Units.feetToMeters(1.0)),
            new Translation2d(Units.feetToMeters(7.0), Units.feetToMeters(27.0)),
            maxVelocityConstraint);

    assertFalse(regionConstraint.isPoseInRegion(new Pose2d()));
    assertTrue(
        regionConstraint.isPoseInRegion(
            new Pose2d(Units.feetToMeters(3.0), Units.feetToMeters(14.5), new Rotation2d())));
  }
}
