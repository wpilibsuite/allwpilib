/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.trajectory;

import java.util.List;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.geometry.Translation2d;
import edu.wpi.first.wpilibj.trajectory.constraint.MaxVelocityConstraint;
import edu.wpi.first.wpilibj.trajectory.constraint.RectangularRegionConstraint;
import edu.wpi.first.wpilibj.util.Units;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class RectangularRegionConstraintTest {
  @Test
  void testConstraint() {
    // Create constraints
    double maxVelocity = Units.feetToMeters(3.0);
    var maxVelocityConstraint = new MaxVelocityConstraint(maxVelocity);
    var regionConstraint = new RectangularRegionConstraint(
        new Translation2d(Units.feetToMeters(1.0), Units.feetToMeters(1.0)),
        new Translation2d(Units.feetToMeters(7.0), Units.feetToMeters(27.0)),
        maxVelocityConstraint
    );

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
    var regionConstraint = new RectangularRegionConstraint(
        new Translation2d(Units.feetToMeters(1.0), Units.feetToMeters(1.0)),
        new Translation2d(Units.feetToMeters(7.0), Units.feetToMeters(27.0)),
        maxVelocityConstraint
    );

    assertFalse(regionConstraint.isPoseInRegion(new Pose2d()));
    assertTrue(regionConstraint.isPoseInRegion(new Pose2d(Units.feetToMeters(3.0),
        Units.feetToMeters(14.5), new Rotation2d())));
  }
}
