// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rectangle2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufRectangle2d;
import org.junit.jupiter.api.Test;

class Rectangle2dProtoTest {
  private static final Rectangle2d DATA =
      new Rectangle2d(new Pose2d(0.1, 0.2, new Rotation2d()), 4.0, 3.0);

  @Test
  void testRoundtrip() {
    ProtobufRectangle2d proto = Rectangle2d.proto.createMessage();
    Rectangle2d.proto.pack(proto, DATA);

    Rectangle2d data = Rectangle2d.proto.unpack(proto);
    assertEquals(DATA.getCenter(), data.getCenter());
    assertEquals(DATA.getXWidth(), data.getXWidth());
    assertEquals(DATA.getYWidth(), data.getYWidth());
  }
}
