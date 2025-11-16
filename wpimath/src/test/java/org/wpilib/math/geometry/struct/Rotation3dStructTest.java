// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation3d;

class Rotation3dStructTest {
  private static final Rotation3d DATA = new Rotation3d(new Quaternion(2.29, 0.191, 0.191, 17.4));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Rotation3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Rotation3d.struct.pack(buffer, DATA);
    buffer.rewind();

    Rotation3d data = Rotation3d.struct.unpack(buffer);
    assertEquals(DATA.getQuaternion(), data.getQuaternion());
  }
}
