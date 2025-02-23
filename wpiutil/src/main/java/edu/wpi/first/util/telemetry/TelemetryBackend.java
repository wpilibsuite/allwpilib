// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import java.util.function.BiConsumer;

/** Interface for telemetry backends. */
public interface TelemetryBackend extends AutoCloseable {
  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  TelemetryEntry getEntry(String path);

  /**
   * Set function used for reporting warning messages (e.g. type mismatches).
   *
   * @param func reporting function
   */
  void setReportWarning(BiConsumer<String, StackTraceElement[]> func);
}
