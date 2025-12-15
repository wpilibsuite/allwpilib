// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

/** The base interface for complex telemetry objects. */
@FunctionalInterface
public interface TelemetryLoggable {
  /**
   * Logs the object to a {@link TelemetryTable}.
   *
   * @param table telemetry table
   */
  void logTo(TelemetryTable table);

  /**
   * Gets the telemetry table type. Default is no specified table type (null). A mismatch in type
   * during logging results in a reported warning, so this should always return the same value.
   *
   * @return Table type
   */
  default String getTelemetryType() {
    return null;
  }
}
