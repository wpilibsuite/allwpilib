// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufPose3d;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class Pose3dProtoTest {
  private static final Pose3d DATA =
      new Pose3d(1.91, 2.29, 17.4, new Rotation3d(35.4, 12.34, 56.78));

  @Test
  void testProtoPack() {
    ProtobufPose3d proto = Pose3d.proto.createMessage();
    Pose3d.proto.pack(proto, DATA);

    Assertions.assertEquals(
        DATA.getTranslation(), Translation3d.proto.unpack(proto.getTranslation()));
    Assertions.assertEquals(DATA.getRotation(), Rotation3d.proto.unpack(proto.getRotation()));
  }

  @Test
  void testProtoUnpack() {
    ProtobufPose3d proto = Pose3d.proto.createMessage();
    Translation3d.proto.pack(proto.getMutableTranslation(), DATA.getTranslation());
    Rotation3d.proto.pack(proto.getMutableRotation(), DATA.getRotation());

    Pose3d data = Pose3d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
