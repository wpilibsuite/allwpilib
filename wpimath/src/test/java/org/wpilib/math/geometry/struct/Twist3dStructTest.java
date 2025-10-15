// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Twist3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Twist3dStructTest {
  private static final Twist3d DATA = new Twist3d(1.1, 2.29, 35.04, 0.174, 19.1, 4.4);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Twist3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Twist3d.struct.pack(buffer, DATA);
    buffer.rewind();

    Twist3d data = Twist3d.struct.unpack(buffer);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dz, data.dz);
    assertEquals(DATA.rx, data.rx);
    assertEquals(DATA.ry, data.ry);
    assertEquals(DATA.rz, data.rz);
  }
}
