// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.vision.apriltag;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.alibaba.fastjson2.JSON;
import com.alibaba.fastjson2.JSONReader;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.squareup.moshi.Moshi;
import io.avaje.jsonb.Jsonb;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose3d;
import org.wpilib.math.geometry.Rotation3d;
import org.wpilib.math.util.Units;

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

  @Test
  void deserializeMatchesAvaje() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(1, Pose3d.kZero),
                new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero))),
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0));

    var layoutType = Jsonb.instance().type(AprilTagFieldLayout.class);

    var deserialized = assertDoesNotThrow(() -> layoutType.fromJson(layoutType.toJson(layout)));

    assertEquals(layout, deserialized);
  }

  @Test
  void deserializeMatchesMoshi() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(1, Pose3d.kZero),
                new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero))),
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0));

    var layoutType = new Moshi.Builder().build().adapter(AprilTagFieldLayout.class);

    var deserialized = assertDoesNotThrow(() -> layoutType.fromJson(layoutType.toJson(layout)));

    assertEquals(layout, deserialized);
  }

  @Test
  void deserializeMatchesFastjson2() {
    var layout =
        new AprilTagFieldLayout(
            List.of(
                new AprilTag(1, Pose3d.kZero),
                new AprilTag(3, new Pose3d(0, 1, 0, Rotation3d.kZero))),
            Units.feetToMeters(54.0),
            Units.feetToMeters(27.0));

    var deserialized =
        assertDoesNotThrow(
            () -> JSONReader.of(JSON.toJSONString(layout)).read(AprilTagFieldLayout.class));

    assertEquals(layout, deserialized);
  }
}
