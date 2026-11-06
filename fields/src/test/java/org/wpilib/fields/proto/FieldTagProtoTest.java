// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.fields.FieldTag;
import org.wpilib.fields.proto.detail.ProtobufFieldTag;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

class FieldTagProtoTest {
  private static final FieldTag DATA = new FieldTag(3, new Pose3d(0, 1, 0, Rotation3d.ZERO));

  @Test
  void testRoundtrip() {
    ProtobufFieldTag proto = FieldTag.proto.createMessage();
    FieldTag.proto.pack(proto, DATA);

    FieldTag data = FieldTag.proto.unpack(proto);
    assertEquals(DATA.getID(), data.getID());
    assertEquals(DATA.getPose(), data.getPose());
  }
}
