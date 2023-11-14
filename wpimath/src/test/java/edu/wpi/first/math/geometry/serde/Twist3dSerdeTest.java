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
  private static final byte[] STRUCT_BUFFER = createStructBuffer();

  private static final byte[] createStructBuffer() {
    byte[] bytes = new byte[Twist3d.struct.getSize()];
    ByteBuffer buffer = ByteBuffer.wrap(bytes);
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    buffer.putDouble(1.91);
    buffer.putDouble(2.29);
    buffer.putDouble(1.74);
    buffer.putDouble(0.3504);
    buffer.putDouble(0.0191);
    buffer.putDouble(0.0229);
    return bytes;
  }

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
