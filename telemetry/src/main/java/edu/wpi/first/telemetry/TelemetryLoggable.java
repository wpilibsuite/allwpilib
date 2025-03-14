// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.telemetry;

/** The base interface for complex telemetry objects. */
@FunctionalInterface
public interface TelemetryLoggable {
  /**
   * Logs the object to a {@link TelemetryTable}.
   *
   * @param table telemetry table
   * @param first if this is the first time toTelemetry() is being called for this table
   */
  void updateTelemetry(TelemetryTable table, boolean first);
}
