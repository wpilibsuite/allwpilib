// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.vision.apriltag.AprilTag;

class AprilTagStructTest {
  private static final AprilTag DATA = new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(AprilTag.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    AprilTag.struct.pack(buffer, DATA);
    buffer.rewind();

    AprilTag data = AprilTag.struct.unpack(buffer);
    assertEquals(DATA.ID, data.ID);
    assertEquals(DATA.pose, data.pose);
  }
}
