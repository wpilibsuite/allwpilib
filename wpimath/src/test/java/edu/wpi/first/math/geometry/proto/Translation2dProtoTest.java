// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTranslation2d;
import org.junit.jupiter.api.Test;

class Translation2dProtoTest {
  private static final Translation2d DATA = new Translation2d(3.504, 22.9);

  @Test
  void testRoundtrip() {
    ProtobufTranslation2d proto = Translation2d.proto.createMessage();
    Translation2d.proto.pack(proto, DATA);

    Translation2d data = Translation2d.proto.unpack(proto);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
  }
}
