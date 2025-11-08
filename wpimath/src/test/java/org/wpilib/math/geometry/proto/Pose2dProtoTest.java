// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.proto.Geometry2D.ProtobufPose2d;
import org.junit.jupiter.api.Test;

class Pose2dProtoTest {
  private static final Pose2d DATA =
      new Pose2d(new Translation2d(0.191, 2.2), new Rotation2d(22.9));

  @Test
  void testRoundtrip() {
    ProtobufPose2d proto = Pose2d.proto.createMessage();
    Pose2d.proto.pack(proto, DATA);

    Pose2d data = Pose2d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
