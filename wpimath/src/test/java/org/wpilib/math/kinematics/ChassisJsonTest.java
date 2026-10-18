// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import io.avaje.jsonb.Jsonb;
import org.junit.jupiter.api.Test;

class ChassisJsonTest {
  private final Jsonb jsonb = Jsonb.instance();

  @Test
  void testChassisVelocitiesRoundTrip() {
    var type = jsonb.type(ChassisVelocities.class);
    var velocities = new ChassisVelocities(1.5, -2.5, 3.5);

    String json = type.toJson(velocities);

    assertTrue(json.contains("\"vx\""));
    assertTrue(json.contains("\"vy\""));
    assertTrue(json.contains("\"omega\""));

    var deserialized = type.fromJson(json);
    assertEquals(velocities.vx, deserialized.vx);
    assertEquals(velocities.vy, deserialized.vy);
    assertEquals(velocities.omega, deserialized.omega);
  }

  @Test
  void testChassisAccelerationsRoundTrip() {
    var type = jsonb.type(ChassisAccelerations.class);
    var accelerations = new ChassisAccelerations(0.5, -1.5, 2.5);

    String json = type.toJson(accelerations);

    assertTrue(json.contains("\"ax\""));
    assertTrue(json.contains("\"ay\""));
    assertTrue(json.contains("\"alpha\""));

    var deserialized = type.fromJson(json);
    assertEquals(accelerations.ax, deserialized.ax);
    assertEquals(accelerations.ay, deserialized.ay);
    assertEquals(accelerations.alpha, deserialized.alpha);
  }
}
