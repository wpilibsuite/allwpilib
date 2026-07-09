// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.util.Units;
import org.wpilib.vision.apriltag.AprilTag;
import org.wpilib.vision.apriltag.AprilTagFieldLayout;

class AprilTagFieldLayoutProtoTest {
  private static final AprilTagFieldLayout DATA =
      new AprilTagFieldLayout(
          List.of(
              new AprilTag(1, Pose3d.kZero),
              new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero))),
          Units.feetToMeters(54.0),
          Units.feetToMeters(27.0));

  @Test
  void testRoundtrip() {
    ProtobufAprilTagFieldLayout proto = AprilTagFieldLayout.proto.createMessage();
    AprilTagFieldLayout.proto.pack(proto, DATA);

    AprilTagFieldLayout data = AprilTagFieldLayout.proto.unpack(proto);
    assertEquals(DATA.getTagMap(), data.getTagMap());
    assertEquals(DATA.getFieldLength(), data.getFieldLength());
    assertEquals(DATA.getFieldWidth(), data.getFieldWidth());
  }
}
