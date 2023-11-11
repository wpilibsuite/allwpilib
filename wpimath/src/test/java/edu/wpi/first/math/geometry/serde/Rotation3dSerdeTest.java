// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufRotation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Rotation3dSerdeTest {
  private static final Rotation3d DATA = new Rotation3d(1.91, 2.29, 35.04);
  private static final byte[] STRUCT_BUFFER =
      new byte[] {
        10, -88, 124, 104, -38, 70, -27, -65, 95, 75, 113, 113, 97, -20, -30, 63, 11, -40, 66, 111,
        -115, -46, -55, -65, 44, -40, 31, 20, -91, 49, -38, -65
      };

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Rotation3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Rotation3d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Rotation3d data = Rotation3d.struct.unpack(buffer);
    assertEquals(DATA.getQuaternion(), data.getQuaternion());
  }

  @Test
  void testProtoPack() {
    ProtobufRotation3d proto = Rotation3d.proto.createMessage();
    Rotation3d.proto.pack(proto, DATA);

    assertEquals(DATA.getQuaternion(), Quaternion.proto.unpack(proto.getQ()));
  }

  @Test
  void testProtoUnpack() {
    ProtobufRotation3d proto = Rotation3d.proto.createMessage();
    Quaternion.proto.pack(proto.getMutableQ(), DATA.getQuaternion());

    Rotation3d data = Rotation3d.proto.unpack(proto);
    assertEquals(DATA.getQuaternion(), data.getQuaternion());
  }
}
