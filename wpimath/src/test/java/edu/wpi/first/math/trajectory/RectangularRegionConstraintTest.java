// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Rectangle2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.trajectory.constraint.MaxVelocityConstraint;
import edu.wpi.first.math.trajectory.constraint.RectangularRegionConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class RectangularRegionConstraintTest {
  @Test
  void testConstraint() {
    double maxVelocity = Units.feetToMeters(2.0);
    var rectangle =
        new Rectangle2d(
            new Translation2d(Units.feetToMeters(1.0), Units.feetToMeters(1.0)),
            new Translation2d(Units.feetToMeters(5.0), Units.feetToMeters(27.0)));

    var trajectory =
        TrajectoryGeneratorTest.getTrajectory(
            List.of(
                new RectangularRegionConstraint(
                    rectangle, new MaxVelocityConstraint(maxVelocity))));

    boolean exceededConstraintOutsideRegion = false;
    for (var point : trajectory.getStates()) {
      if (rectangle.contains(point.poseMeters.getTranslation())) {
        assertTrue(Math.abs(point.velocityMetersPerSecond) < maxVelocity + 0.05);
      } else if (Math.abs(point.velocityMetersPerSecond) >= maxVelocity + 0.05) {
        exceededConstraintOutsideRegion = true;
      }
    }
    assertTrue(exceededConstraintOutsideRegion);
  }
}
