// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

/** The base interface for complex telemetry objects. */
@FunctionalInterface
public interface TelemetryLoggable {
  /**
   * Logs the object to a {@link TelemetryTable}.
   *
   * @param table telemetry table
   */
  void log(TelemetryTable table);
}
