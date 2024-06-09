// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rectangle2d;
import edu.wpi.first.math.geometry.Rotation2d;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class Rectangle2dStructTest {
  private static final Rectangle2d DATA =
      new Rectangle2d(new Pose2d(1.0, 2.0, new Rotation2d(3.1)), 5.0, 3.0);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Rectangle2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Rectangle2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Rectangle2d data = Rectangle2d.struct.unpack(buffer);
    assertEquals(DATA.getCenter(), data.getCenter());
    assertEquals(DATA.getXWidth(), data.getXWidth());
    assertEquals(DATA.getYWidth(), data.getYWidth());
  }
}
