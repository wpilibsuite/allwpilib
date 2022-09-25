// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.apriltag;

import com.fasterxml.jackson.databind.ObjectMapper;

import org.junit.jupiter.api.Test;

import java.util.List;

import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

class AprilTagSerializationTest {
  @Test
  void deserializeMatches() {
    var layout = new AprilTagFieldLayout(List.of(
            new AprilTag(1, new Pose3d(0, 0, 0, new Rotation3d(0, 0, 0))),
            new AprilTag(3, new Pose3d(0, 1, 0, new Rotation3d(0, 0, 0)))));

    var objectMapper = new ObjectMapper();

    var deserialized =
        assertDoesNotThrow(
            () -> objectMapper.readValue(objectMapper.writeValueAsString(layout), AprilTagFieldLayout.class));

    assertEquals(layout, deserialized);
  }
}
