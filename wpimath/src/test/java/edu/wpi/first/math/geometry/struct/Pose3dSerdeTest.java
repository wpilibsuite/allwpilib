// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Pose3dSerdeTest {
  private static final Pose3d DATA =
      new Pose3d(1.91, 2.29, 17.4, new Rotation3d(35.4, 12.34, 56.78));
  private static final byte[] STRUCT_BUFFER = createStructBuffer();

  private static byte[] createStructBuffer() {
    byte[] bytes = new byte[Pose3d.struct.getSize()];
    ByteBuffer buffer = ByteBuffer.wrap(bytes);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putDouble(1.91);
    buffer.putDouble(2.29);
    buffer.putDouble(17.4);
    buffer.putDouble(DATA.getRotation().getQuaternion().getW());
    buffer.putDouble(DATA.getRotation().getQuaternion().getX());
    buffer.putDouble(DATA.getRotation().getQuaternion().getY());
    buffer.putDouble(DATA.getRotation().getQuaternion().getZ());
    return bytes;
  }

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Pose3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Pose3d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Pose3d data = Pose3d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
