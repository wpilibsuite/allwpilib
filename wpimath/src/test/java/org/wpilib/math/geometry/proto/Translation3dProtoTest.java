// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation3d;
import org.wpilib.math.proto.ProtobufTranslation3d;

class Translation3dProtoTest {
  private static final Translation3d DATA = new Translation3d(35.04, 22.9, 3.504);

  @Test
  void testRoundtrip() {
    ProtobufTranslation3d proto = Translation3d.proto.createMessage();
    Translation3d.proto.pack(proto, DATA);

    Translation3d data = Translation3d.proto.unpack(proto);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
