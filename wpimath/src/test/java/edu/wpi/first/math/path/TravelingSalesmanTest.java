// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.path;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.util.CircularBuffer;
import org.junit.jupiter.api.Test;

class TravelingSalesmanTest {
  /**
   * Returns true if the cycles represented by the two lists match.
   *
   * @param expected The expected cycle.
   * @param actual The actual cycle.
   */
  private boolean isMatchingCycle(Pose2d[] expected, Pose2d[] actual) {
    assertEquals(expected.length, actual.length);

    // Check actual has expected cycle (forward)
    var actualBufferForward = new CircularBuffer<Pose2d>(actual.length);
    for (int i = 0; i < actual.length; ++i) {
      actualBufferForward.addLast(actual[i % actual.length]);
    }
    boolean matchesExpectedForward = true;
    for (int i = 0; i < expected.length; ++i) {
      matchesExpectedForward &= expected[i].equals(actualBufferForward.get(i));
    }

    // Check actual has expected cycle (reverse)
    var actualBufferReverse = new CircularBuffer<Pose2d>(actual.length);
    for (int i = 0; i < actual.length; ++i) {
      actualBufferReverse.addFirst(actual[(1 + i) % actual.length]);
    }

    boolean matchesExpectedReverse = true;
    for (int i = 0; i < expected.length; ++i) {
      matchesExpectedReverse &= expected[i].equals(actualBufferReverse.get(i));
    }

    // Actual may be reversed from expected, but that's still valid
    return matchesExpectedForward || matchesExpectedReverse;
  }

  @Test
  void testFiveLengthPathWithDistanceCost() {
    // ...................
    // ........2..........
    // ..0..........4.....
    // ...................
    // ....3.....1........
    // ...................
    var poses =
        new Pose2d[] {
          new Pose2d(3, 3, Rotation2d.kZero),
          new Pose2d(11, 5, Rotation2d.kZero),
          new Pose2d(9, 2, Rotation2d.kZero),
          new Pose2d(5, 5, Rotation2d.kZero),
          new Pose2d(14, 3, Rotation2d.kZero)
        };

    var traveler = new TravelingSalesman();
    var solution = traveler.solve(poses, 500);

    assertEquals(5, solution.length);
    var expected = new Pose2d[] {poses[0], poses[2], poses[4], poses[1], poses[3]};
    assertTrue(isMatchingCycle(expected, solution));
  }

  @Test
  void testTenLengthPathWithDistanceCost() {
    // ....6.3..1.2.......
    // ..4................
    // .............9.....
    // .0.................
    // .....7..5...8......
    // ...................
    var poses =
        new Pose2d[] {
          new Pose2d(2, 4, Rotation2d.kZero),
          new Pose2d(10, 1, Rotation2d.kZero),
          new Pose2d(12, 1, Rotation2d.kZero),
          new Pose2d(7, 1, Rotation2d.kZero),
          new Pose2d(3, 2, Rotation2d.kZero),
          new Pose2d(9, 5, Rotation2d.kZero),
          new Pose2d(5, 1, Rotation2d.kZero),
          new Pose2d(6, 5, Rotation2d.kZero),
          new Pose2d(13, 5, Rotation2d.kZero),
          new Pose2d(14, 3, Rotation2d.kZero)
        };

    var traveler = new TravelingSalesman();
    var solution = traveler.solve(poses, 500);

    assertEquals(10, solution.length);
    var expected =
        new Pose2d[] {
          poses[0], poses[4], poses[6], poses[3], poses[1], poses[2], poses[9], poses[8], poses[5],
          poses[7]
        };
    assertTrue(isMatchingCycle(expected, solution));
  }
}
