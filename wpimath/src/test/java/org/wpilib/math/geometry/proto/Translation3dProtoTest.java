// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTranslation3d;
import org.junit.jupiter.api.Test;

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
