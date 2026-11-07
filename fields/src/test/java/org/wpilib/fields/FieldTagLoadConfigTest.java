// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.fields;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.math.util.UnitConversions.inchesToMeters;

import java.util.Optional;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;

class FieldTagLoadConfigTest {
  @ParameterizedTest
  @EnumSource(Fields.class)
  void testLoad(Fields field) {
    if (field.hasTags) {
      Field loadedField = Assertions.assertDoesNotThrow(() -> Field.loadField(field));
      assertNotNull(loadedField);
    }
  }

  @Test
  void test2022RapidReact() {
    Field field = Field.loadField(Fields.FRC_2022_RAPID_REACT);

    // Blue Hangar Truss - Hub
    Pose3d expectedPose =
        new Pose3d(
            inchesToMeters(196.728),
            inchesToMeters(-54.01),
            inchesToMeters(67.932),
            new Rotation3d(0, 0, Math.PI));
    Optional<Pose3d> maybePose = field.getTagPose(1);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Blue Terminal Near Station
    expectedPose =
        new Pose3d(
            inchesToMeters(319.232),
            inchesToMeters(94.369),
            inchesToMeters(35.063),
            new Rotation3d(0, 0, Math.toRadians(226.25)));
    maybePose = field.getTagPose(5);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Upper Hub Blue-Near
    expectedPose =
        new Pose3d(
            inchesToMeters(-8.321),
            inchesToMeters(-21.676),
            inchesToMeters(95.186),
            new Rotation3d(0, Math.toRadians(26.75), Math.toRadians(249)));
    maybePose = field.getTagPose(53);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Doesn't exist
    maybePose = field.getTagPose(54);
    assertFalse(maybePose.isPresent());
  }
}
