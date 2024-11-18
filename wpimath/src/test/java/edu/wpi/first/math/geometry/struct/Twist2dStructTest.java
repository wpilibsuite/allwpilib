// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Twist2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Twist2dStructTest {
  private static final Twist2d DATA = new Twist2d(2.29, 35.04, 35.04);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Twist2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Twist2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Twist2d data = Twist2d.struct.unpack(buffer);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dtheta, data.dtheta);
  }
}
