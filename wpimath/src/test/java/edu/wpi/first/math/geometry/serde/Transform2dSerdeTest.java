// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufTransform2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Transform2dSerdeTest {
  private static final Transform2d DATA =
      new Transform2d(new Translation2d(1.91, 2.29), Rotation2d.fromDegrees(35.04));
  private static final byte[] STRUCT_BUFFER =
      new byte[] {
        -113, -62, -11, 40, 92, -113, -2, 63, 82, -72, 30, -123, -21, 81, 2, 64, -41, -77, 36, 88,
        -19, -111, -29, 63
      };

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Transform2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Transform2d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Transform2d data = Transform2d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }

  @Test
  void testProtoPack() {
    ProtobufTransform2d proto = Transform2d.proto.createMessage();
    Transform2d.proto.pack(proto, DATA);

    assertEquals(DATA.getTranslation(), Translation2d.proto.unpack(proto.getTranslation()));
    assertEquals(DATA.getRotation(), Rotation2d.proto.unpack(proto.getRotation()));
  }

  @Test
  void testProtoUnpack() {
    ProtobufTransform2d proto = Transform2d.proto.createMessage();
    Translation2d.proto.pack(proto.getMutableTranslation(), DATA.getTranslation());
    Rotation2d.proto.pack(proto.getMutableRotation(), DATA.getRotation());

    Transform2d data = Transform2d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
