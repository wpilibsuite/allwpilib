// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import java.util.List;
import org.junit.jupiter.api.Test;

class TrajectoryConcatenateTest {
  @Test
  void testStates() {
    var t1 =
        TrajectoryGenerator.generateTrajectory(
            Pose2d.ZERO, List.of(), new Pose2d(1, 1, Rotation2d.ZERO), new TrajectoryConfig(2, 2));

    var t2 =
        TrajectoryGenerator.generateTrajectory(
            new Pose2d(1, 1, Rotation2d.ZERO),
            List.of(),
            new Pose2d(2, 2, Rotation2d.fromDegrees(45)),
            new TrajectoryConfig(2, 2));

    var t = t1.concatenate(t2);

    double time = -1.0;
    for (int i = 0; i < t.getStates().size(); ++i) {
      var state = t.getStates().get(i);

      // Make sure that the timestamps are strictly increasing.
      assertTrue(state.time > time);
      time = state.time;

      // Ensure that the states in t are the same as those in t1 and t2.
      if (i < t1.getStates().size()) {
        assertEquals(state, t1.getStates().get(i));
      } else {
        var st = t2.getStates().get(i - t1.getStates().size() + 1);
        st.time += t1.getTotalTime();
        assertEquals(state, st);
      }
    }
  }
}
