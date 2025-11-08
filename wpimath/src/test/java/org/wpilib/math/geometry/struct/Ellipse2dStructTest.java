// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Ellipse2d;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;

class Ellipse2dStructTest {
  private static final Ellipse2d DATA =
      new Ellipse2d(new Pose2d(0.0, 1.0, new Rotation2d(2.4)), 3.0, 4.0);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(Ellipse2d.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    Ellipse2d.struct.pack(buffer, DATA);
    buffer.rewind();

    Ellipse2d data = Ellipse2d.struct.unpack(buffer);
    assertEquals(DATA.getCenter(), data.getCenter());
    assertEquals(DATA.getXSemiAxis(), data.getXSemiAxis());
    assertEquals(DATA.getYSemiAxis(), data.getYSemiAxis());
  }
}
