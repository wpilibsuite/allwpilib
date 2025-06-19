// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufPose3d;
import org.junit.jupiter.api.Test;

class Pose3dProtoTest {
  private static final Pose3d DATA = new Pose3d(
      new Translation3d(1.1, 2.2, 1.1), new Rotation3d(new Quaternion(1.91, 0.3504, 3.3, 1.74)));

  @Test
  void testRoundtrip() {
    ProtobufPose3d proto = Pose3d.proto.createMessage();
    Pose3d.proto.pack(proto, DATA);

    Pose3d data = Pose3d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
