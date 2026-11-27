// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.fields.FieldImage;
import org.wpilib.fields.proto.detail.ProtobufFieldImage;

class FieldImageProtoTest {
  private static final FieldImage DATA =
      new FieldImage("frc/2026-rebuilt.png", 121, 254, 1917, 3951);

  @Test
  void testRoundtrip() {
    ProtobufFieldImage proto = FieldImage.proto.createMessage();
    FieldImage.proto.pack(proto, DATA);

    FieldImage data = FieldImage.proto.unpack(proto);
    assertEquals(DATA.path, data.path);
    assertEquals(DATA.top, data.top);
    assertEquals(DATA.left, data.left);
    assertEquals(DATA.bottom, data.bottom);
    assertEquals(DATA.right, data.right);
  }
}
