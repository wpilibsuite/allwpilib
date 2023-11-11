// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTranslation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Translation2dSerdeTest {
  private static final Translation2d DATA = new Translation2d(1.91, 2.29);
  private static final byte[] STRUCT_BUFFER =
      new byte[] {-113, -62, -11, 40, 92, -113, -2, 63, 82, -72, 30, -123, -21, 81, 2, 64};

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Translation2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Translation2d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Translation2d data = Translation2d.struct.unpack(buffer);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
  }

  @Test
  void testProtoPack() {
    ProtobufTranslation2d proto = Translation2d.proto.createMessage();
    Translation2d.proto.pack(proto, DATA);

    assertEquals(DATA.getX(), proto.getXMeters());
    assertEquals(DATA.getY(), proto.getYMeters());
  }

  @Test
  void testProtoUnpack() {
    ProtobufTranslation2d proto = Translation2d.proto.createMessage();
    proto.setXMeters(DATA.getX());
    proto.setYMeters(DATA.getY());

    Translation2d data = Translation2d.proto.unpack(proto);
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
  }
}
