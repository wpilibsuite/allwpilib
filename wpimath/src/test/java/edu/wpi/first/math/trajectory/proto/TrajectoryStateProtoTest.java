// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Trajectory.ProtobufTrajectoryState;
import edu.wpi.first.math.trajectory.Trajectory;
import org.junit.jupiter.api.Test;

class TrajectoryStateProtoTest {
  private static final Trajectory.State DATA =
      new Trajectory.State(
          1.91, 4.4, 17.4, new Pose2d(new Translation2d(1.74, 19.1), new Rotation2d(22.9)), 0.174);

  @Test
  void testRoundtrip() {
    ProtobufTrajectoryState proto = Trajectory.State.proto.createMessage();
    Trajectory.State.proto.pack(proto, DATA);

    Trajectory.State data = Trajectory.State.proto.unpack(proto);
    assertEquals(DATA.time, data.time);
    assertEquals(DATA.velocity, data.velocity);
    assertEquals(DATA.acceleration, data.acceleration);
    assertEquals(DATA.pose, data.pose);
    assertEquals(DATA.curvature, data.curvature);
  }
}
