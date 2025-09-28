// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import java.util.List;

/**
 * Metadata associated with a logged data point that provides context about dependencies or
 * relationships with other logged values. This is particularly useful for analysis tools like
 * AdvantageScope to understand complex conditions and triggers.
 *
 * @param dependencies the names of fields/methods that this log entry depends on
 */
public record LogMetadata(List<String> dependencies) {
  /** Empty metadata instance for logging that doesn't have dependencies. */
  public static final LogMetadata kEmpty = new LogMetadata(List.of());

  /**
   * Creates metadata with a list of dependency names.
   *
   * @param dependencies the names of fields/methods that this log entry depends on
   */
  public LogMetadata(List<String> dependencies) {
    this.dependencies = List.copyOf(dependencies); // Create immutable copy
  }
}
