// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Twist2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTwist2d;
import org.junit.jupiter.api.Test;

class Twist2dProtoTest {
  private static final Twist2d DATA = new Twist2d(2.29, 35.04, 35.04);

  @Test
  void testRoundtrip() {
    ProtobufTwist2d proto = Twist2d.proto.createMessage();
    Twist2d.proto.pack(proto, DATA);

    Twist2d data = Twist2d.proto.unpack(proto);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dtheta, data.dtheta);
  }
}
