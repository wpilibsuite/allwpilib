// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.trajectory;

import java.util.List;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveKinematicsConstraint;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class TrajectoryJsonTest {
  @Test
  void deserializeMatches() {
    var config = List.of(new DifferentialDriveKinematicsConstraint(
        new DifferentialDriveKinematics(20), 3));
    var trajectory = TrajectoryGeneratorTest.getTrajectory(config);

    var deserialized =
        assertDoesNotThrow(() ->
            TrajectoryUtil.deserializeTrajectory(TrajectoryUtil.serializeTrajectory(trajectory)));

    assertEquals(trajectory.getStates(), deserialized.getStates());
  }
}
