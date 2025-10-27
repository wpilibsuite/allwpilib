// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.proto.Geometry3D.ProtobufQuaternion;
import org.junit.jupiter.api.Test;

class QuaternionProtoTest {
  private static final Quaternion DATA = new Quaternion(1.1, 0.191, 35.04, 19.1);

  @Test
  void testRoundtrip() {
    ProtobufQuaternion proto = Quaternion.proto.createMessage();
    Quaternion.proto.pack(proto, DATA);

    Quaternion data = Quaternion.proto.unpack(proto);
    assertEquals(DATA.getW(), data.getW());
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
