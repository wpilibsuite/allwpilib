// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTransform3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

class Transform3dSerdeTest {
  private static final Transform3d DATA =
      new Transform3d(new Translation3d(1.91, 2.29, 1.74), new Rotation3d(1.1, 2.2, 3.3));
  private static final byte[] STRUCT_BUFFER =
      new byte[] {
        -113, -62, -11, 40, 92, -113, -2, 63, 82, -72, 30, -123, -21, 81, 2, 64, -41, -93, 112, 61,
        10, -41, -5, 63, 103, -6, -36, 49, -45, -62, -37, 63, 41, -88, -17, -82, 62, -42, -24, -65,
        110, 94, -118, -72, -64, -114, -58, 63, 36, 52, 84, -57, -74, 7, -37, 63
      };

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Transform3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Transform3d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Transform3d data = Transform3d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }

  @Test
  void testProtoPack() {
    ProtobufTransform3d proto = Transform3d.proto.createMessage();
    Transform3d.proto.pack(proto, DATA);

    Assertions.assertEquals(
        DATA.getTranslation(), Translation3d.proto.unpack(proto.getTranslation()));
    Assertions.assertEquals(DATA.getRotation(), Rotation3d.proto.unpack(proto.getRotation()));
  }

  @Test
  void testProtoUnpack() {
    ProtobufTransform3d proto = Transform3d.proto.createMessage();
    Translation3d.proto.pack(proto.getMutableTranslation(), DATA.getTranslation());
    Rotation3d.proto.pack(proto.getMutableRotation(), DATA.getRotation());

    Transform3d data = Transform3d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
