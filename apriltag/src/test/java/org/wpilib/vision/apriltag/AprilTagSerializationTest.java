// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.databind.ObjectMapper;
import edu.wpi.first.math.geometry.Pose3d;
import edu.wpi.first.math.geometry.Rotation3d;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class AprilTagSerializationTest {
  @Test
  void deserializeMatches() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(1, Pose3d.kZero),
                new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero))),
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0));

    var objectMapper = new ObjectMapper();

    var deserialized =
        assertDoesNotThrow(
            () ->
                objectMapper.readValue(
                    objectMapper.writeValueAsString(layout), AprilTagFieldLayout.class));

    assertEquals(layout, deserialized);
  }
}
