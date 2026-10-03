// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.wpilib.math.util.UnitConversions.degreesToRadians;
import static org.wpilib.math.util.UnitConversions.feetToMeters;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.geometry.Translation3d;

class FieldTagPoseSetOriginTest {
  @Test
  void transformationMatches() {
    var field =
        new Field(
            "2027 FRC Test Field",
            "2027",
            "Test Field",
            null,
            feetToMeters(54.0),
            feetToMeters(27.0),
            "frc",
            List.of(
                new FieldTag(1, Pose3d.ZERO),
                new FieldTag(
                    2,
                    new Pose3d(
                        new Translation3d(feetToMeters(4.0), feetToMeters(4), feetToMeters(4)),
                        new Rotation3d(0, 0, degreesToRadians(180))))));
    field.setOrigin(Field.OriginPosition.RED_ALLIANCE_WALL_RIGHT_SIDE);

    assertEquals(
        new Pose3d(
            new Translation3d(feetToMeters(54.0), feetToMeters(27.0), 0.0),
            new Rotation3d(0.0, 0.0, degreesToRadians(180.0))),
        field.getTagPose(1).orElse(null));

    assertEquals(
        new Pose3d(
            new Translation3d(feetToMeters(50.0), feetToMeters(23.0), feetToMeters(4)),
            Rotation3d.ZERO),
        field.getTagPose(2).orElse(null));
  }
}
