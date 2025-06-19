// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.geometry.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Quaternion;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.geometry.Transform3d;
import edu.wpi.first.math.geometry.Translation3d;
import edu.wpi.first.math.proto.Geometry3D.ProtobufTransform3d;
import org.junit.jupiter.api.Test;

class Transform3dProtoTest {
  private static final Transform3d DATA = new Transform3d(
      new Translation3d(0.3504, 22.9, 3.504),
      new Rotation3d(new Quaternion(0.3504, 35.04, 2.29, 0.3504)));

  @Test
  void testRoundtrip() {
    ProtobufTransform3d proto = Transform3d.proto.createMessage();
    Transform3d.proto.pack(proto, DATA);

    Transform3d data = Transform3d.proto.unpack(proto);
    assertEquals(DATA.getTranslation(), data.getTranslation());
    assertEquals(DATA.getRotation(), data.getRotation());
  }
}
