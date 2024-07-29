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
        new Pose2d(1, 2, Rotation2d.fromDegrees(30)), transformedTrajectory.sample(0).poseMeters);

    testSameShapedTrajectory(trajectory.getStates(), transformedTrajectory.getStates());
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
    assertEquals(Pose2d.kZero, transformedTrajectory.sample(0).poseMeters);

    testSameShapedTrajectory(trajectory.getStates(), transformedTrajectory.getStates());
  }

  void testSameShapedTrajectory(List<Trajectory.State> statesA, List<Trajectory.State> statesB) {
    for (int i = 0; i < statesA.size() - 1; i++) {
      var a1 = statesA.get(i).poseMeters;
      var a2 = statesA.get(i + 1).poseMeters;

      var b1 = statesB.get(i).poseMeters;
      var b2 = statesB.get(i + 1).poseMeters;

      assertEquals(a2.relativeTo(a1), b2.relativeTo(b1));
    }
  }
}
