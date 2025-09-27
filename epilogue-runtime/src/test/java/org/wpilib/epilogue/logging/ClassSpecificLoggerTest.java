// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.epilogue.Logged;
import java.util.List;
import org.junit.jupiter.api.Test;

class ClassSpecificLoggerTest {
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
}
