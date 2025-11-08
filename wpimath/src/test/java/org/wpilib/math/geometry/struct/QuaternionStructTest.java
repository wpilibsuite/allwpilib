// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Quaternion;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class QuaternionStructTest {
  private static final Quaternion DATA = new Quaternion(1.1, 0.191, 35.04, 19.1);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Quaternion.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Quaternion.struct.pack(buffer, DATA);
    buffer.rewind();

    Quaternion data = Quaternion.struct.unpack(buffer);
    assertEquals(DATA.getW(), data.getW());
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
