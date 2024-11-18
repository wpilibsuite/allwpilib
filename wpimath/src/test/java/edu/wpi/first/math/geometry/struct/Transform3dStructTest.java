// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.geometry.Quaternion;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Transform3d;
import org.wpilib.math.geometry.Translation3d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Transform3dStructTest {
  private static final Transform3d DATA =
      new Transform3d(
          new Translation3d(0.3504, 22.9, 3.504),
          new Rotation3d(new Quaternion(0.3504, 35.04, 2.29, 0.3504)));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Transform3d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Transform3d.struct.pack(buffer, DATA);
    buffer.rewind();

    Transform3d data = Transform3d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
