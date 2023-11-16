// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTwist3d;
import org.junit.jupiter.api.Test;

class Twist3dProtoTest {
  private static final Twist3d DATA = new Twist3d(1.1, 2.29, 35.04, 0.174, 19.1, 4.4);

  @Test
  void testRoundtrip() {
    ProtobufTwist3d proto = Twist3d.proto.createMessage();
    Twist3d.proto.pack(proto, DATA);

    Twist3d data = Twist3d.proto.unpack(proto);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dz, data.dz);
    assertEquals(DATA.rx, data.rx);
    assertEquals(DATA.ry, data.ry);
    assertEquals(DATA.rz, data.rz);
  }
}
