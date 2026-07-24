// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.epilogue.Logged;
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
      protected void update(EpilogueBackend backend, TelemetryValue object) {
        logTelemetry(backend.getNested("telemetry"), object);
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
      protected void update(EpilogueBackend backend, Point2d object) {
        backend.log("x", object.x);
        backend.log("y", object.y);
        backend.log("dim", object.dim);
      }
    }
  }

  @Test
  void testReadPrivate() {
    var point = new Point2d(1, 4, 2);
    var logger = new Point2d.Logger();
    var dataLog = new TestBackend();
    logger.update(dataLog.getNested("Point"), point);

    assertEquals(
        List.of(
            new TestBackend.LogEntry<>("Point/x", 1.0),
            new TestBackend.LogEntry<>("Point/y", 4.0),
            new TestBackend.LogEntry<>("Point/dim", 2)),
        dataLog.getEntries());
  }

  @Test
  void logsTelemetryLoggable() {
    var logger = new TelemetryValue.Logger();
    var backend = new TestBackend();
    logger.update(backend, new TelemetryValue(3));

    assertEquals(
        List.of(
            new TestBackend.LogEntry<>("telemetry/value", 3L),
            new TestBackend.LogEntry<>("telemetry/child/enabled", true),
            new TestBackend.LogEntry<>("telemetry/.type", "TelemetryValue")),
        backend.getEntries());
  }
}
