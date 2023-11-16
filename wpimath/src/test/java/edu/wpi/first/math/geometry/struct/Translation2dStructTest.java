// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Translation2dStructTest {
  private static final Translation2d DATA = new Translation2d(3.504, 22.9);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Translation2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Translation2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Translation2d data = Translation2d.struct.unpack(buffer);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
  }
}
