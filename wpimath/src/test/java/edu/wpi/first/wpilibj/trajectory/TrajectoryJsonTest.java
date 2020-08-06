/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
