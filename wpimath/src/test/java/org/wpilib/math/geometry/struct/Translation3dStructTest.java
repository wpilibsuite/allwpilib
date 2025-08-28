// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Translation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Translation3dStructTest {
  private static final Translation3d DATA = new Translation3d(35.04, 22.9, 3.504);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Translation3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Translation3d.struct.pack(buffer, DATA);
    buffer.rewind();

    Translation3d data = Translation3d.struct.unpack(buffer);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
