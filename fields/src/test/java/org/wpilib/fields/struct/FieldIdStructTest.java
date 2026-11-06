// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.fields.Fields;

class FieldIdStructTest {
  private static final Fields DATA = Fields.DEFAULT_FIELD;

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Fields.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Fields.struct.pack(buffer, DATA);
    buffer.rewind();

    Fields data = Fields.struct.unpack(buffer);
    assertEquals(DATA, data);
  }
}
