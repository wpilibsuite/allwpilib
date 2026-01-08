// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Ellipse2d;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.proto.ProtobufEllipse2d;

class Ellipse2dProtoTest {
  private static final Ellipse2d DATA =
      new Ellipse2d(new Pose2d(1.0, 3.6, new Rotation2d(4.1)), 2.0, 1.0);

  @Test
  void testRoundtrip() {
    ProtobufEllipse2d proto = Ellipse2d.proto.createMessage();
    Ellipse2d.proto.pack(proto, DATA);

    Ellipse2d data = Ellipse2d.proto.unpack(proto);
    assertEquals(DATA.getCenter(), data.getCenter());
    assertEquals(DATA.getXSemiAxis(), data.getXSemiAxis());
    assertEquals(DATA.getYSemiAxis(), data.getYSemiAxis());
  }
}
