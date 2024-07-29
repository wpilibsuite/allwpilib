// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.geometry.Translation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Transform2dStructTest {
  private static final Transform2d DATA =
      new Transform2d(new Translation2d(0.191, 2.2), new Rotation2d(4.4));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Transform2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Transform2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Transform2d data = Transform2d.struct.unpack(buffer);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
