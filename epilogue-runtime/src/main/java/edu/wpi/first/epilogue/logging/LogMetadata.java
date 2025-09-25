// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import java.util.List;
import java.util.Objects;

/**
 * Metadata associated with a logged data point that provides context about dependencies
 * or relationships with other logged values. This is particularly useful for analysis
 * tools like AdvantageScope to understand complex conditions and triggers.
 */
public final class LogMetadata {
  private final List<String> dependencies;

  /**
   * Creates metadata with a list of dependency names.
   *
   * @param dependencies the names of fields/methods that this log entry depends on
   */
  public LogMetadata(List<String> dependencies) {
    this.dependencies = List.copyOf(dependencies); // Create immutable copy
  }

  /**
   * Gets the list of dependency names.
   *
   * @return an immutable list of dependency names
   */
  public List<String> getDependencies() {
    return dependencies;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null || getClass() != obj.getClass()) return false;
    LogMetadata that = (LogMetadata) obj;
    return Objects.equals(dependencies, that.dependencies);
  }

  @Override
  public int hashCode() {
    return Objects.hash(dependencies);
  }

  @Override
  public String toString() {
    return "LogMetadata{dependencies=" + dependencies + "}";
  }
}