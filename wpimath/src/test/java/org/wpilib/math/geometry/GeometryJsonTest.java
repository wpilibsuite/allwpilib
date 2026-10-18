// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.geometry;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import io.avaje.jsonb.Jsonb;
import org.junit.jupiter.api.Test;

/** Pins the JSON wire format of the geometry classes now that they expose public final fields. */
class GeometryJsonTest {
  private final Jsonb jsonb = Jsonb.instance();

  @Test
  void testTranslation2d() {
    var type = jsonb.type(Translation2d.class);
    var value = new Translation2d(1.5, -2.5);

    String json = type.toJson(value);
    assertTrue(json.contains("\"x\""));
    assertTrue(json.contains("\"y\""));

    var decoded = type.fromJson(json);
    assertEquals(value.x, decoded.x);
    assertEquals(value.y, decoded.y);
    assertEquals(value, type.fromJson("{\"x\":1.5,\"y\":-2.5}"));
  }

  @Test
  void testTranslation3d() {
    var type = jsonb.type(Translation3d.class);
    var value = new Translation3d(1.5, -2.5, 3.5);

    String json = type.toJson(value);
    assertTrue(json.contains("\"x\""));
    assertTrue(json.contains("\"y\""));
    assertTrue(json.contains("\"z\""));

    assertEquals(value, type.fromJson(json));
    assertEquals(value, type.fromJson("{\"x\":1.5,\"y\":-2.5,\"z\":3.5}"));
  }

  @Test
  void testRotation2dSerializesRadiansOnly() {
    var type = jsonb.type(Rotation2d.class);
    var value = Rotation2d.fromDegrees(30);

    String json = type.toJson(value);
    assertTrue(json.contains("\"radians\""));
    assertFalse(json.contains("\"cos\""));
    assertFalse(json.contains("\"sin\""));

    var decoded = type.fromJson(json);
    assertEquals(value.cos, decoded.cos, 1e-12);
    assertEquals(value.sin, decoded.sin, 1e-12);
    assertEquals(0.5, type.fromJson("{\"radians\":0.5}").getRadians(), 1e-12);
  }

  @Test
  void testQuaternionUsesUppercaseNames() {
    var type = jsonb.type(Quaternion.class);
    var value = new Quaternion(1.0, 2.0, 3.0, 4.0);

    String json = type.toJson(value);
    for (String key : new String[] {"\"W\"", "\"X\"", "\"Y\"", "\"Z\""}) {
      assertTrue(json.contains(key), key);
    }
    for (String key : new String[] {"\"w\"", "\"x\"", "\"y\"", "\"z\""}) {
      assertFalse(json.contains(key), key);
    }

    var decoded = type.fromJson("{\"W\":1.0,\"X\":2.0,\"Y\":3.0,\"Z\":4.0}");
    assertEquals(1.0, decoded.w);
    assertEquals(2.0, decoded.x);
    assertEquals(3.0, decoded.y);
    assertEquals(4.0, decoded.z);
  }

  @Test
  void testRotation3d() {
    var type = jsonb.type(Rotation3d.class);
    var value = new Rotation3d(0.1, 0.2, 0.3);

    String json = type.toJson(value);
    assertTrue(json.contains("\"quaternion\""));

    assertEquals(value, type.fromJson(json));
  }

  @Test
  void testPose2d() {
    var type = jsonb.type(Pose2d.class);
    var value = new Pose2d(1.0, 2.0, Rotation2d.fromDegrees(45));

    String json = type.toJson(value);
    assertTrue(json.contains("\"translation\""));
    assertTrue(json.contains("\"rotation\""));

    assertEquals(value, type.fromJson(json));
  }

  @Test
  void testPose3d() {
    var type = jsonb.type(Pose3d.class);
    var value = new Pose3d(1.0, 2.0, 3.0, new Rotation3d(0.1, 0.2, 0.3));

    String json = type.toJson(value);
    assertTrue(json.contains("\"translation\""));
    assertTrue(json.contains("\"rotation\""));

    assertEquals(value, type.fromJson(json));
  }
}
