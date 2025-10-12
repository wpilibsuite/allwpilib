// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import java.util.List;
import org.junit.jupiter.api.Test;

class TrajectoryTransformTest {
  @Test
  void testTransformBy() {
    var config = new TrajectoryConfig(3, 3);
    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            Pose2d.kZero, List.of(), new Pose2d(1, 1, Rotation2d.kCCW_Pi_2), config);

    var transformedTrajectory =
        trajectory.transformBy(
            new Transform2d(new Translation2d(1, 2), Rotation2d.fromDegrees(30)));

    // Test initial pose.
    assertEquals(
        new Pose2d(1, 2, Rotation2d.fromDegrees(30)), transformedTrajectory.sampleAt(0).pose);

    testSameShapedTrajectory(trajectory.samples, transformedTrajectory.samples);
  }

  @Test
  void testRelativeTo() {
    var config = new TrajectoryConfig(3, 3);
    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            new Pose2d(1, 2, Rotation2d.fromDegrees(30.0)),
            List.of(),
            new Pose2d(5, 7, Rotation2d.kCCW_Pi_2),
            config);

    var transformedTrajectory = trajectory.relativeTo(new Pose2d(1, 2, Rotation2d.fromDegrees(30)));

    // Test initial pose.
    assertEquals(Pose2d.kZero, transformedTrajectory.sampleAt(0).pose);

    testSameShapedTrajectory(trajectory.samples, transformedTrajectory.samples);
  }

  void testSameShapedTrajectory(SplineSample[] statesA, SplineSample[] statesB) {
    for (int i = 0; i < statesA.length - 1; i++) {
      var a1 = statesA[i].pose;
      var a2 = statesA[i + 1].pose;

      var b1 = statesB[i].pose;
      var b2 = statesB[i + 1].pose;

      var expectedRel = a2.relativeTo(a1);
      var actualRel = b2.relativeTo(b1);
      
      // Use tolerance-based comparison for floating point precision
      assertEquals(expectedRel.getX(), actualRel.getX(), 1e-2, 
        String.format("X mismatch at index %d: expected %s, actual %s", i, expectedRel, actualRel));
      assertEquals(expectedRel.getY(), actualRel.getY(), 1e-2, 
        String.format("Y mismatch at index %d: expected %s, actual %s", i, expectedRel, actualRel));
      assertEquals(expectedRel.getRotation().getRadians(), actualRel.getRotation().getRadians(), 1e-6, 
        String.format("Rotation mismatch at index %d: expected %s, actual %s", i, expectedRel, actualRel));
    }
  }
}
