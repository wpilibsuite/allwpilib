// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import org.junit.jupiter.api.Test;

public class AprilTagTest {
  @Test
  void protobufTest() {
    var tag = new AprilTag(0, new Pose3d());
    var packedTag = AprilTag.proto.createMessage();
    AprilTag.proto.pack(packedTag, tag);
    var unpackedTag = AprilTag.proto.unpack(packedTag);
    assertEquals(tag, unpackedTag);
  }
}
