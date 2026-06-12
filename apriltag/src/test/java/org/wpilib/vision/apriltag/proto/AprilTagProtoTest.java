// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.vision.apriltag.AprilTag;
import org.wpilib.vision.apriltag.proto.Apriltag.ProtobufAprilTag;

class AprilTagProtoTest {
  private static final AprilTag DATA = new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero));

  @Test
  void testRoundtrip() {
    ProtobufAprilTag proto = AprilTag.proto.createMessage();
    AprilTag.proto.pack(proto, DATA);

    AprilTag data = AprilTag.proto.unpack(proto);
    assertEquals(DATA.ID, data.ID);
    assertEquals(DATA.pose, data.pose);
  }
}
