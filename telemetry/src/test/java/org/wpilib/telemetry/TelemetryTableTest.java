// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TelemetryTableTest {
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

  record Thing(double x, double y) implements TelemetryLoggable {
    @Override
    public void updateTelemetry(TelemetryTable table) {
      table.log("x", x);
      table.log("y", y);
    }
  }

  record ThingType(double x, double y) implements TelemetryLoggable {
    @Override
    public void updateTelemetry(TelemetryTable table) {
      table.log("x", x);
      table.log("y", y);
    }

    @Override
    public String getTelemetryType() {
      return "Thing";
    }
  }

  @Test
  void testLoggable() {
    TelemetryTable table = TelemetryRegistry.getTable("/");
    Thing val = new Thing(1, 2);
    table.log("test", val);
    List<MockTelemetryBackend.Action> actions = m_mock.getActions();
    assertEquals(actions.size(), 2);

    assertEquals(actions.get(0).path(), "/test/x");
    assertTrue(actions.get(0).value() instanceof Double);
    assertEquals((Double) actions.get(0).value(), 1);

    assertEquals(actions.get(1).path(), "/test/y");
    assertTrue(actions.get(1).value() instanceof Double);
    assertEquals((Double) actions.get(1).value(), 2);
  }

  @Test
  void testLoggableType() {
    TelemetryTable table = TelemetryRegistry.getTable("/");
    ThingType val = new ThingType(1, 2);
    table.log("test", val);
    assertEquals(table.getTable("test").getType(), "Thing");
    table.log("test", val);
    List<MockTelemetryBackend.Action> actions = m_mock.getActions();
    assertEquals(actions.size(), 5);

    var value = m_mock.getLastValue("/test/.type", MockTelemetryBackend.LogStringValue.class);
    assertEquals(value.value(), "Thing");
  }
}
