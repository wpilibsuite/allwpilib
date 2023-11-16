// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Rotation2dStructTest {
  private static final Rotation2d DATA = new Rotation2d(1.91);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Rotation2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Rotation2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Rotation2d data = Rotation2d.struct.unpack(buffer);
    assertEquals(DATA.getRadians(), data.getRadians());
  }
}
