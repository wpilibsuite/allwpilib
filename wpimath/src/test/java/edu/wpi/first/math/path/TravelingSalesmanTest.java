// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.path;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.Vector;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.numbers.N5;
import org.junit.jupiter.api.Test;

class TravelingSalesmanTest {
  @Test
  void testFiveLengthPathWithDistanceCost() {
    Pose2d[] poses = new Pose2d[5];
    poses[0] = new Pose2d(3, 3, new Rotation2d(0));
    poses[2] = new Pose2d(9, 2, new Rotation2d(0));
    poses[4] = new Pose2d(14, 3, new Rotation2d(0));
    poses[1] = new Pose2d(11, 5, new Rotation2d(0));
    poses[3] = new Pose2d(5, 5, new Rotation2d(0));
    // ...................
    // ........2..........
    // ..0..........4.....
    // ...................
    // ....3.....1........
    // ...................

    TravelingSalesman<N5> traveler =
        new TravelingSalesman<>(TravelingSalesman.distanceCostFunction, poses);

    Vector<N5> sol = traveler.solve(1000);

    assertTrue(traveler.evaluate(sol) < 24);
  }

  @Test
  void testTenLengthPathWithDistanceCost() {
    Pose2d[] poses = new Pose2d[10];
    poses[0] = new Pose2d(2, 4, new Rotation2d(0));
    poses[1] = new Pose2d(10, 1, new Rotation2d(0));
    poses[2] = new Pose2d(12, 1, new Rotation2d(0));
    poses[3] = new Pose2d(7, 1, new Rotation2d(0));
    poses[4] = new Pose2d(3, 2, new Rotation2d(0));
    poses[5] = new Pose2d(9, 5, new Rotation2d(0));
    poses[6] = new Pose2d(5, 1, new Rotation2d(0));
    poses[7] = new Pose2d(6, 5, new Rotation2d(0));
    poses[8] = new Pose2d(13, 5, new Rotation2d(0));
    poses[9] = new Pose2d(14, 3, new Rotation2d(0));
    // ....6.3..1.2.......
    // ..4................
    // .............9.....
    // .0.................
    // .....7..5...8......
    // ...................

    TravelingSalesman<N5> traveler =
        new TravelingSalesman<>(TravelingSalesman.distanceCostFunction, poses);

    Vector<N5> sol = traveler.solve(1000);

    assertTrue(traveler.evaluate(sol) < 28);
  }
}
