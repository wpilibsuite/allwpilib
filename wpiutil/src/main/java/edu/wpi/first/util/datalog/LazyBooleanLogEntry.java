// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/**
 * Lazy log boolean values. This will only write if the value has been changed since last append
 * call
 */
public class LazyBooleanLogEntry extends BooleanLogEntry {
  private boolean m_lastValue;
  private boolean m_firstAppend = true;

  public LazyBooleanLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, metadata, timestamp);
  }

  public LazyBooleanLogEntry(DataLog log, String name, String metadata) {
    super(log, name, metadata);
  }

  public LazyBooleanLogEntry(DataLog log, String name, long timestamp) {
    super(log, name, timestamp);
  }

  public LazyBooleanLogEntry(DataLog log, String name) {
    super(log, name);
  }

  @Override
  public void append(boolean value, long timestamp) {
    if (m_firstAppend || value != m_lastValue) {
      m_firstAppend = false;
      m_lastValue = value;
      super.append(value, timestamp);
    }
  }
}
