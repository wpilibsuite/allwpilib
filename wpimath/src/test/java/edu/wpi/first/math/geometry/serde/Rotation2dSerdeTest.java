// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufRotation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Rotation2dSerdeTest {
  private static final Rotation2d DATA = new Rotation2d(35.04);
  private static final byte[] STRUCT_BUFFER = createStructBuffer();

  private static final byte[] createStructBuffer() {
    byte[] bytes = new byte[Rotation2d.struct.getSize()];
    ByteBuffer buffer = ByteBuffer.wrap(bytes);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putDouble(35.04);
    return bytes;
  }

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Rotation2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Rotation2d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Rotation2d data = Rotation2d.struct.unpack(buffer);
    assertEquals(DATA.getRadians(), data.getRadians());
  }

  @Test
  void testProtoPack() {
    ProtobufRotation2d proto = Rotation2d.proto.createMessage();
    Rotation2d.proto.pack(proto, DATA);

    assertEquals(DATA.getRadians(), proto.getRadians());
  }

  @Test
  void testProtoUnpack() {
    ProtobufRotation2d proto = Rotation2d.proto.createMessage();
    proto.setRadians(DATA.getRadians());

    Rotation2d data = Rotation2d.proto.unpack(proto);
    assertEquals(DATA.getRadians(), data.getRadians());
  }
}
