// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.epilogue.Logged;
import org.wpilib.telemetry.MockTelemetryBackend;
import org.wpilib.telemetry.TelemetryLoggable;
import org.wpilib.telemetry.TelemetryTable;

class ClassSpecificLoggerTest {
  record TelemetryValue(int value) implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      table.log("value", value);
      table.getTable("child").log("enabled", true);
    }

    @Override
    public String getTelemetryType() {
      return "TelemetryValue";
    }

    static class Logger extends ClassSpecificLogger<TelemetryValue> {
      Logger() {
        super(TelemetryValue.class);
      }

      @Override
      protected void update(TelemetryTable table, TelemetryValue object) {
        table.log("telemetry", object);
      }
    }
  }

  @Logged
  record Point2d(double x, double y, int dim) {
    static class Logger extends ClassSpecificLogger<Point2d> {
      Logger() {
        super(Point2d.class);
      }

      @Override
      protected void update(TelemetryTable table, Point2d object) {
        table.log("x", object.x);
        table.log("y", object.y);
        table.log("dim", object.dim);
      }
    }
  }

  @Test
  void testReadPrivate() {
    var point = new Point2d(1, 4, 2);
    var logger = new Point2d.Logger();
    var backend = new MockTelemetryBackend();
    logger.update(new TelemetryTable(backend).getTable("Point"), point);

    assertEquals(
        List.of(
            new MockTelemetryBackend.Action("/Point/x", 1.0),
            new MockTelemetryBackend.Action("/Point/y", 4.0),
            new MockTelemetryBackend.Action("/Point/dim", 2)),
        backend.getActions());
  }

  @Test
  void logsTelemetryLoggable() {
    var logger = new TelemetryValue.Logger();
    var backend = new MockTelemetryBackend();
    logger.update(new TelemetryTable(backend), new TelemetryValue(3));

    assertEquals(
        List.of(
            new MockTelemetryBackend.Action(
                "/telemetry/.type",
                new MockTelemetryBackend.LogStringValue("TelemetryValue", "string")),
            new MockTelemetryBackend.Action("/telemetry/value", 3),
            new MockTelemetryBackend.Action("/telemetry/child/enabled", true)),
        backend.getActions());
  }
}
