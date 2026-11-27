// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.fields.Fields;
import org.wpilib.fields.proto.detail.ProtobufFieldId;

class FieldIdProtoTest {
  private static final Fields DATA = Fields.DEFAULT_FIELD;

  @Test
  void testRoundtrip() {
    ProtobufFieldId proto = Fields.proto.createMessage();
    Fields.proto.pack(proto, DATA);

    Fields data = Fields.proto.unpack(proto);
    assertEquals(DATA, data);
  }
}
