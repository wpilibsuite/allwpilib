// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import java.util.List;

import edu.wpi.first.units.measure.Time;
import org.junit.jupiter.api.Test;

class TrajectoryConcatenateTest {
  @Test
  void testStates() {
    var t1 =
        TrajectoryGenerator.generateTrajectory(
            Pose2d.kZero,
            List.of(),
            new Pose2d(1, 1, Rotation2d.kZero),
            new TrajectoryConfig(2, 2));

    var t2 =
        TrajectoryGenerator.generateTrajectory(
            new Pose2d(1, 1, Rotation2d.kZero),
            List.of(),
            new Pose2d(2, 2, Rotation2d.fromDegrees(45)),
            new TrajectoryConfig(2, 2));

    var t = t1.concatenate(t2);

    Time t1Duration = t1.duration;

    double time = -1.0;
    for (int i = 0; i < t.samples.length; ++i) {
      var state = t.samples[i];

      // Make sure that the timestamps are strictly increasing.
      assertTrue(state.timestamp.in(Seconds) >= time);
      time = state.timestamp.in(Seconds);

      // Ensure that the states in t are the same as those in t1 and t2.
      if (i < t1.samples.length) {
        assertEquals(state, t1.samples[i]);
      } else {
        // For the second trajectory, we need to account for the offset
        var originalIndex = i - t1.samples.length;
        if (originalIndex < t2.samples.length) {
          var st = t2.samples[originalIndex];
          assertEquals(state.timestamp, st.timestamp.plus(t1Duration));
          assertEquals(state.pose, st.pose);
          assertEquals(state.velocity, st.velocity);
          assertEquals(state.acceleration, st.acceleration);
        }
      }
    }
  }
}
