// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Trajectory.ProtobufTrajectory;
import edu.wpi.first.math.trajectory.Trajectory;
import java.util.List;
import org.junit.jupiter.api.Test;

class TrajectoryProtoTest {
  private static final Trajectory DATA =
      new Trajectory(
          List.of(
              new Trajectory.State(
                  1.1, 2.2, 3.3, new Pose2d(new Translation2d(1.1, 2.2), new Rotation2d(2.2)), 6.6),
              new Trajectory.State(
                  2.1, 2.2, 3.3, new Pose2d(new Translation2d(2.1, 2.2), new Rotation2d(2.2)), 6.6),
              new Trajectory.State(
                  3.1,
                  2.2,
                  3.3,
                  new Pose2d(new Translation2d(3.1, 2.2), new Rotation2d(2.2)),
                  6.6)));

  @Test
  void testRoundtrip() {
    ProtobufTrajectory proto = Trajectory.proto.createMessage();
    Trajectory.proto.pack(proto, DATA);

    Trajectory data = Trajectory.proto.unpack(proto);
    assertEquals(DATA.getStates(), data.getStates());
  }
}
