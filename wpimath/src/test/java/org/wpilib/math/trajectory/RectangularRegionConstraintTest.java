// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.util.UnitConversions.feetToMeters;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.shape.Rectangle2d;
import org.wpilib.math.trajectory.constraint.MaxVelocityConstraint;
import org.wpilib.math.trajectory.constraint.RectangularRegionConstraint;

class RectangularRegionConstraintTest {
  @Test
  void testConstraint() {
    double maxVelocity = feetToMeters(2.0);
    var rectangle =
        new Rectangle2d(
            new Translation2d(feetToMeters(1.0), feetToMeters(1.0)),
            new Translation2d(feetToMeters(5.0), feetToMeters(27.0)));

    var trajectory =
        DrivetrainSplineTrajectoryGeneratorTest.getTrajectory(
            List.of(
                new RectangularRegionConstraint(
                    rectangle, new MaxVelocityConstraint(maxVelocity))));

    boolean exceededConstraintOutsideRegion = false;
    for (var point : trajectory.samples) {
      if (rectangle.contains(point.pose.translation)) {
        assertTrue(Math.abs(point.forwardVelocity()) < maxVelocity + 0.05);
      } else if (Math.abs(point.forwardVelocity()) >= maxVelocity + 0.05) {
        exceededConstraintOutsideRegion = true;
      }
    }
    assertTrue(exceededConstraintOutsideRegion);
  }
}
