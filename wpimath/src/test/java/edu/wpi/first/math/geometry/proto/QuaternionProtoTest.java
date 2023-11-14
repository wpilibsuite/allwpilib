// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.proto.Geometry3D.ProtobufQuaternion;
import org.junit.jupiter.api.Test;

class QuaternionProtoTest {
  private static final Quaternion DATA = new Quaternion(1.1, 2.2, 3.3, 4.4);

  @Test
  void testProtoPack() {
    ProtobufQuaternion proto = Quaternion.proto.createMessage();
    Quaternion.proto.pack(proto, DATA);

    assertEquals(DATA.getW(), proto.getW());
    assertEquals(DATA.getX(), proto.getX());
    assertEquals(DATA.getY(), proto.getY());
    assertEquals(DATA.getZ(), proto.getZ());
  }

  @Test
  void testProtoUnpack() {
    ProtobufQuaternion proto = Quaternion.proto.createMessage();
    proto.setW(DATA.getW());
    proto.setX(DATA.getX());
    proto.setY(DATA.getY());
    proto.setZ(DATA.getZ());

    Quaternion data = Quaternion.proto.unpack(proto);
    assertEquals(DATA.getW(), data.getW());
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
