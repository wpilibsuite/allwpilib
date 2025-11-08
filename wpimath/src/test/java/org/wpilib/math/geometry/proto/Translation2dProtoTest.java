// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.proto.Geometry2D.ProtobufTranslation2d;

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
