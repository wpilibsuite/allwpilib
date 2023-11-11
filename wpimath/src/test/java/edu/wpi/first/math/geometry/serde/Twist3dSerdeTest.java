// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.serde;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Twist3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTwist3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Twist3dSerdeTest {
  private static final Twist3d DATA = new Twist3d(1.91, 2.29, 1.74, 0.3504, 0.0191, 0.0229);
  private static final byte[] STRUCT_BUFFER =
      new byte[] {
        -113, -62, -11, 40, 92, -113, -2, 63, 82, -72, 30, -123, -21, 81, 2, 64, -41, -93, 112, 61,
        10, -41, -5, 63, 119, 45, 33, 31, -12, 108, -42, 63, 30, 22, 106, 77, -13, -114, -109, 63,
        23, 72, 80, -4, 24, 115, -105, 63
      };

  @Test
  void testStructPack() {
    ByteBuffer buffer = ByteBuffer.allocate(Twist3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Twist3d.struct.pack(buffer, DATA);

    byte[] actual = buffer.array();
    assertArrayEquals(actual, STRUCT_BUFFER);
  }

  @Test
  void testStructUnpack() {
    ByteBuffer buffer = ByteBuffer.wrap(STRUCT_BUFFER);
    buffer.order(ByteOrder.LITTLE_ENDIAN);

    Twist3d data = Twist3d.struct.unpack(buffer);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dz, data.dz);
    assertEquals(DATA.rx, data.rx);
    assertEquals(DATA.ry, data.ry);
    assertEquals(DATA.rz, data.rz);
  }

  @Test
  void testProtoPack() {
    ProtobufTwist3d proto = Twist3d.proto.createMessage();
    Twist3d.proto.pack(proto, DATA);

    assertEquals(DATA.dx, proto.getDxMeters());
    assertEquals(DATA.dy, proto.getDyMeters());
    assertEquals(DATA.dz, proto.getDzMeters());
    assertEquals(DATA.rx, proto.getRxRadians());
    assertEquals(DATA.ry, proto.getRyRadians());
    assertEquals(DATA.rz, proto.getRzRadians());
  }

  @Test
  void testProtoUnpack() {
    ProtobufTwist3d proto = Twist3d.proto.createMessage();
    proto.setDxMeters(DATA.dx);
    proto.setDyMeters(DATA.dy);
    proto.setDzMeters(DATA.dz);
    proto.setRxRadians(DATA.rx);
    proto.setRyRadians(DATA.ry);
    proto.setRzRadians(DATA.rz);

    Twist3d data = Twist3d.proto.unpack(proto);
    assertEquals(DATA.dx, data.dx);
    assertEquals(DATA.dy, data.dy);
    assertEquals(DATA.dz, data.dz);
    assertEquals(DATA.rx, data.rx);
    assertEquals(DATA.ry, data.ry);
    assertEquals(DATA.rz, data.rz);
  }
}
