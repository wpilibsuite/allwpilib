// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class QuaternionSerdeTest {
  private static final Quaternion DATA = new Quaternion(1.1, 2.2, 3.3, 4.4);
  private static final byte[] STRUCT_BUFFER = createStructBuffer();

  private static byte[] createStructBuffer() {
    byte[] bytes = new byte[Quaternion.struct.getSize()];
    ByteBuffer buffer = ByteBuffer.wrap(bytes);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putDouble(1.1);
    buffer.putDouble(2.2);
    buffer.putDouble(3.3);
    buffer.putDouble(4.4);
    return bytes;
  }

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Quaternion.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Quaternion.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Quaternion data = Quaternion.struct.unpack(buffer);
    assertEquals(DATA.getW(), data.getW());
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
