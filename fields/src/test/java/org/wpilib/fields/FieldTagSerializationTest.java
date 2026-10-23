// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import io.avaje.jsonb.Jsonb;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.util.Units;

class FieldTagSerializationTest {
  @Test
  void deserializeMatches() {
    var field =
        new Field(
            "2027 FRC Test Field",
            "2027",
            "Test Field",
            null,
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0),
            "frc",
            List.of(
                new FieldTag(1, Pose3d.ZERO),
                new FieldTag(3, new Pose3d(0, 1, 0, Rotation3d.ZERO))));

    var fieldType = Jsonb.instance().type(Field.class);

    var deserialized = assertDoesNotThrow(() -> fieldType.fromJson(fieldType.toJson(field)));

    assertEquals(field, deserialized);
  }
}
