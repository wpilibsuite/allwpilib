// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.util.Units;
import java.util.Optional;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class LoadConfigTest {
  @ParameterizedTest
  @EnumSource(AprilTagFields.class)
  void testLoad(AprilTagFields field) {
    AprilTagFieldLayout layout =
        Assertions.assertDoesNotThrow(() -> AprilTagFieldLayout.loadField(field));
    assertNotNull(layout);
  }

  @Test
  void test2022RapidReact() {
    AprilTagFieldLayout layout = AprilTagFieldLayout.loadField(AprilTagFields.k2022RapidReact);

    // Blue Hangar Truss - Hub
    Pose3d expectedPose =
        new Pose3d(
            Units.inchesToMeters(127.272),
            Units.inchesToMeters(216.01),
            Units.inchesToMeters(67.932),
            Rotation3d.kZero);
    Optional<Pose3d> maybePose = layout.getTagPose(1);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Blue Terminal Near Station
    expectedPose =
        new Pose3d(
            Units.inchesToMeters(4.768),
            Units.inchesToMeters(67.631),
            Units.inchesToMeters(35.063),
            new Rotation3d(0, 0, Math.toRadians(46.25)));
    maybePose = layout.getTagPose(5);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Upper Hub Blue-Near
    expectedPose =
        new Pose3d(
            Units.inchesToMeters(332.321),
            Units.inchesToMeters(183.676),
            Units.inchesToMeters(95.186),
            new Rotation3d(0, Math.toRadians(26.75), Math.toRadians(69)));
    maybePose = layout.getTagPose(53);
    assertTrue(maybePose.isPresent());
    assertEquals(expectedPose, maybePose.get());

    // Doesn't exist
    maybePose = layout.getTagPose(54);
    assertFalse(maybePose.isPresent());
  }
}
