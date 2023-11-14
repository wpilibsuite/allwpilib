// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufRotation3d;
import org.junit.jupiter.api.Test;

class Rotation3dProtoTest {
  private static final Rotation3d DATA = new Rotation3d(1.91, 2.29, 35.04);

  @Test
  void testProtoPack() {
    ProtobufRotation3d proto = Rotation3d.proto.createMessage();
    Rotation3d.proto.pack(proto, DATA);

    assertEquals(DATA.getQuaternion(), Quaternion.proto.unpack(proto.getQuaternion()));
  }

  @Test
  void testProtoUnpack() {
    ProtobufRotation3d proto = Rotation3d.proto.createMessage();
    Quaternion.proto.pack(proto.getMutableQuaternion(), DATA.getQuaternion());

    Rotation3d data = Rotation3d.proto.unpack(proto);
    assertEquals(DATA.getQuaternion(), data.getQuaternion());
  }
}
