// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.proto.Geometry3D.ProtobufQuaternion;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class QuaternionSerdeTest {
  private static final Quaternion DATA = new Quaternion(1.1, 2.2, 3.3, 4.4);
  private static final byte[] STRUCT_BUFFER =
      new byte[] {
        -102, -103, -103, -103, -103, -103, -15, 63, -102, -103, -103, -103, -103, -103, 1, 64, 102,
        102, 102, 102, 102, 102, 10, 64, -102, -103, -103, -103, -103, -103, 17, 64
      };

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

  @Test
  void testProtoPack() {
    ProtobufQuaternion proto = Quaternion.proto.createMessage();
    Quaternion.proto.pack(proto, DATA);

    assertEquals(DATA.getW(), proto.getW());
    assertEquals(DATA.getX(), proto.getX());
    assertEquals(DATA.getY(), proto.getY());
    assertEquals(DATA.getZ(), proto.getZ());
  }

  @Test
  void testProtoUnpack() {
    ProtobufQuaternion proto = Quaternion.proto.createMessage();
    proto.setW(DATA.getW());
    proto.setX(DATA.getX());
    proto.setY(DATA.getY());
    proto.setZ(DATA.getZ());

    Quaternion data = Quaternion.proto.unpack(proto);
    assertEquals(DATA.getW(), data.getW());
    assertEquals(DATA.getX(), data.getX());
    assertEquals(DATA.getY(), data.getY());
    assertEquals(DATA.getZ(), data.getZ());
  }
}
