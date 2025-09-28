// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.geometry.Transform2d;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.proto.Geometry2D.ProtobufTransform2d;

class Transform2dProtoTest {
  private static final Transform2d DATA =
      new Transform2d(new Translation2d(0.191, 2.2), new Rotation2d(4.4));

  @Test
  void testRoundtrip() {
    ProtobufTransform2d proto = Transform2d.proto.createMessage();
    Transform2d.proto.pack(proto, DATA);

    Transform2d data = Transform2d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
