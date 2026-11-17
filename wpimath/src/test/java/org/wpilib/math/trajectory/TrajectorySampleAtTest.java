// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;

class TrajectorySampleAtTest {
  private static DrivetrainSplineSample splineSample(
      double time, double x, double velocity, double acceleration) {
    return new DrivetrainSplineSample(
        time, new Pose2d(x, 0.0, Rotation2d.ZERO), velocity, acceleration, 0.0);
  }

  private static HolonomicSample holonomicSample(double time, double ax) {
    return new HolonomicSample(
        time, Pose2d.ZERO, new ChassisVelocities(), new ChassisAccelerations(ax, 0.0, 0.0));
  }

  @Test
  void testExactTimestampReturnsStoredSample() {
    // Waits at x = 0 from 0 s to 1 s, then accelerates to x = 1 at 2 s. The first segment has
    // zero length, so interpolating to its far end divides by zero.
    var trajectory =
        new DrivetrainSplineTrajectory(
            List.of(
                splineSample(0.0, 0.0, 0.0, 0.0),
                splineSample(1.0, 0.0, 0.0, 2.0),
                splineSample(2.0, 1.0, 2.0, 0.0)));

    var sample = trajectory.sampleAt(1.0);

    assertFalse(Double.isNaN(sample.pose.getX()));
    assertEquals(trajectory.getSamples().get(1), sample);
  }

  @Test
  void testExactTimestampOfEverySample() {
    var trajectory =
        new HolonomicTrajectory(
            List.of(
                holonomicSample(0.0, 0.0),
                holonomicSample(1.0, 1.0),
                holonomicSample(2.0, 2.0),
                holonomicSample(3.0, 3.0)));

    for (var expected : trajectory.getSamples()) {
      assertEquals(expected, trajectory.sampleAt(expected.time));
    }
  }

  @Test
  void testJoinReturnsSecondTrajectoryStart() {
    var first =
        new HolonomicTrajectory(List.of(holonomicSample(0.0, 0.0), holonomicSample(1.0, 0.0)));
    var second =
        new HolonomicTrajectory(List.of(holonomicSample(0.0, 2.0), holonomicSample(1.0, 0.0)));

    var joined = first.concatenate(second);

    // Both trajectories contribute a sample at t = 1 s. The last one wins, so the feedforward at
    // the join comes from the start of the second trajectory.
    assertEquals(4, joined.getSamples().size());
    assertEquals(2.0, joined.sampleAt(1.0).acceleration.ax, 1e-9);
  }

  @Test
  void testOutOfRangeAndEdgeTimes() {
    var trajectory =
        new HolonomicTrajectory(List.of(holonomicSample(0.0, 1.0), holonomicSample(1.0, 2.0)));

    assertSame(trajectory.start(), trajectory.sampleAt(-1.0));
    assertSame(trajectory.start(), trajectory.sampleAt(-0.0));
    assertSame(trajectory.start(), trajectory.sampleAt(0.0));
    assertSame(trajectory.end(), trajectory.sampleAt(1.0));
    assertSame(trajectory.end(), trajectory.sampleAt(5.0));
  }

  @Test
  void testNaNTimeReturnsASample() {
    var trajectory =
        new HolonomicTrajectory(List.of(holonomicSample(0.0, 1.0), holonomicSample(1.0, 2.0)));

    // Garbage in, but it must not index outside the samples.
    var sample = trajectory.sampleAt(Double.NaN);
    assertTrue(sample.equals(trajectory.start()) || sample.equals(trajectory.end()));
  }
}
