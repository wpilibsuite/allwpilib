// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.util.UnitConversions.feetToMeters;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.fields.Field;
import org.wpilib.fields.FieldTag;
import org.wpilib.fields.proto.detail.ProtobufField;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

class FieldProtoTest {
  private static final Field DATA =
      new Field(
          "Test Field",
          "",
          "",
          null,
          feetToMeters(54.0),
          feetToMeters(27.0),
          "Test",
          List.of(
              new FieldTag(1, Pose3d.ZERO), new FieldTag(3, new Pose3d(0, 1, 0, Rotation3d.ZERO))));

  @Test
  void testRoundtrip() {
    ProtobufField proto = Field.proto.createMessage();
    Field.proto.pack(proto, DATA);

    Field data = Field.proto.unpack(proto);
    assertEquals(DATA.getTags(), data.getTags());
    assertEquals(DATA.getFieldLength(), data.getFieldLength());
    assertEquals(DATA.getFieldWidth(), data.getFieldWidth());
  }
}
