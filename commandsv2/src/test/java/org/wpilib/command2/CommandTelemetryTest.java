// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.telemetry.MockTelemetryBackend;
import org.wpilib.telemetry.TelemetryRegistry;

class CommandTelemetryTest extends CommandTestBase {
  private MockTelemetryBackend m_backend;

  @BeforeEach
  void setUpTelemetry() {
    TelemetryRegistry.reset();
    m_backend = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("", m_backend);
  }

  @AfterEach
  void tearDownTelemetry() {
    TelemetryRegistry.reset();
  }

  @Test
  void commandLogsMetadataWithoutLeadingDots() {
    Command command = new Command() {};
    command.setName("renamed");

    command.logTo(TelemetryRegistry.getTable("command"));

    var name = m_backend.getLastValue("/command/name", MockTelemetryBackend.LogStringValue.class);
    assertNotNull(name);
    assertEquals("renamed", name.value());
    assertNull(m_backend.getLastAction("/command/.name"));

    var isParented = m_backend.getLastValue("/command/isParented", Boolean.class);
    assertNotNull(isParented);
    assertFalse(isParented);
    assertNull(m_backend.getLastAction("/command/.isParented"));
  }
}
