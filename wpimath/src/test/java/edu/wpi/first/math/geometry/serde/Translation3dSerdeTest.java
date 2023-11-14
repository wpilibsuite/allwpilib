// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTranslation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Translation3dSerdeTest {
  private static final Translation3d DATA = new Translation3d(19.1, 22.9, 3.504);
  private static final byte[] STRUCT_BUFFER = createStructBuffer();

  private static final byte[] createStructBuffer() {
    byte[] bytes = new byte[Translation3d.struct.getSize()];
    ByteBuffer buffer = ByteBuffer.wrap(bytes);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putDouble(19.1);
    buffer.putDouble(22.9);
    buffer.putDouble(3.504);
    return bytes;
  }

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Translation3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Translation3d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Translation3d data = Translation3d.struct.unpack(buffer);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }

  @Test
  void testProtoPack() {
    ProtobufTranslation3d proto = Translation3d.proto.createMessage();
    Translation3d.proto.pack(proto, DATA);

    assertEquals(DATA.getX(), proto.getXMeters());
    assertEquals(DATA.getY(), proto.getYMeters());
    assertEquals(DATA.getZ(), proto.getZMeters());
  }

  @Test
  void testProtoUnpack() {
    ProtobufTranslation3d proto = Translation3d.proto.createMessage();
    proto.setXMeters(DATA.getX());
    proto.setYMeters(DATA.getY());
    proto.setZMeters(DATA.getZ());

    Translation3d data = Translation3d.proto.unpack(proto);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
