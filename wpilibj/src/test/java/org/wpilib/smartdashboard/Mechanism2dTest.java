// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.smartdashboard;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.telemetry.MockTelemetryBackend;
import org.wpilib.telemetry.MockTelemetryBackend.LogStringValue;
import org.wpilib.telemetry.MockTelemetryBackend.LogStructValue;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.util.Color8Bit;

class Mechanism2dTest {
  MockTelemetryBackend m_mock;

  @BeforeEach
  public void init() {
    m_mock = new MockTelemetryBackend();
    TelemetryRegistry.reset();
    TelemetryRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TelemetryRegistry.reset();
    m_mock.close();
  }

  @Test
  void testCanvas() {
    try (var mechanism = new Mechanism2d(5, 10)) {
      Telemetry.log("mechanism", mechanism);
      assertArrayEquals(
          new double[] {5, 10}, m_mock.getLastValue("/mechanism/dims", double[].class));
      assertEquals(
          "#000020",
          m_mock.getLastValue("/mechanism/backgroundColor", LogStringValue.class).value());

      mechanism.setBackgroundColor(new Color8Bit(255, 255, 255));
      Telemetry.log("mechanism", mechanism);
      assertEquals(
          "#FFFFFF",
          m_mock.getLastValue("/mechanism/backgroundColor", LogStringValue.class).value());
    }
  }

  @Test
  void testRoot() {
    try (var mechanism = new Mechanism2d(5, 10)) {
      final var root = mechanism.getRoot("root", 1, 2);
      Telemetry.log("mechanism", mechanism);
      assertArrayEquals(
          new double[] {1, 2}, m_mock.getLastValue("/mechanism/root/position", double[].class));

      root.setPosition(2, 4);
      Telemetry.log("mechanism", mechanism);
      assertArrayEquals(
          new double[] {2, 4}, m_mock.getLastValue("/mechanism/root/position", double[].class));
    }
  }

  @Test
  void testLigament() {
    try (var mechanism = new Mechanism2d(5, 10)) {
      var root = mechanism.getRoot("root", 1, 2);
      var ligament =
          root.append(new MechanismLigament2d("ligament", 3, 90, 1, new Color8Bit(255, 255, 255)));
      Telemetry.log("mechanism", mechanism);
      assertEquals(
          ligament.getAngle(),
          m_mock.getLastValue("/mechanism/root/ligament/angle", LogStructValue.class).value());
      assertEquals(
          ligament.getColor().toHexString(),
          m_mock.getLastValue("/mechanism/root/ligament/color", LogStringValue.class).value());
      assertEquals(
          ligament.getLength(),
          m_mock.getLastValue("/mechanism/root/ligament/length", Double.class));
      assertEquals(
          ligament.getLineWeight(),
          m_mock.getLastValue("/mechanism/root/ligament/weight", Double.class));

      ligament.setAngle(45);
      ligament.setColor(new Color8Bit(0, 0, 0));
      ligament.setLength(2);
      ligament.setLineWeight(4);
      Telemetry.log("mechanism", mechanism);
      assertEquals(
          ligament.getAngle(),
          m_mock.getLastValue("/mechanism/root/ligament/angle", LogStructValue.class).value());
      assertEquals(
          ligament.getColor().toHexString(),
          m_mock.getLastValue("/mechanism/root/ligament/color", LogStringValue.class).value());
      assertEquals(
          ligament.getLength(),
          m_mock.getLastValue("/mechanism/root/ligament/length", Double.class));
      assertEquals(
          ligament.getLineWeight(),
          m_mock.getLastValue("/mechanism/root/ligament/weight", Double.class));
    }
  }
}
