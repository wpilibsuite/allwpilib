// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufRotation2d;
import org.junit.jupiter.api.Test;

class Rotation2dProtoTest {
  private static final Rotation2d DATA = new Rotation2d(35.04);

  @Test
  void testProtoPack() {
    ProtobufRotation2d proto = Rotation2d.proto.createMessage();
    Rotation2d.proto.pack(proto, DATA);

    assertEquals(DATA.getRadians(), proto.getRadians());
  }

  @Test
  void testProtoUnpack() {
    ProtobufRotation2d proto = Rotation2d.proto.createMessage();
    proto.setRadians(DATA.getRadians());

    Rotation2d data = Rotation2d.proto.unpack(proto);
    assertEquals(DATA.getRadians(), data.getRadians());
  }
}
