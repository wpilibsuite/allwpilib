// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.fields.FieldTag;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

class FieldTagStructTest {
  private static final FieldTag DATA = new FieldTag(3, new Pose3d(0, 1, 0, Rotation3d.ZERO));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(FieldTag.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    FieldTag.struct.pack(buffer, DATA);
    buffer.rewind();

    FieldTag data = FieldTag.struct.unpack(buffer);
    assertEquals(DATA.getID(), data.getID());
    assertEquals(DATA.getPose(), data.getPose());
  }
}
