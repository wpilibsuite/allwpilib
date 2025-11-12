// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

/** Interface for telemetry backends. */
public interface TelemetryBackend extends AutoCloseable {
  /**
   * Create an entry for the given path.
   *
   * @param path full name
   * @return telemetry entry
   */
  TelemetryEntry getEntry(String path);
}
