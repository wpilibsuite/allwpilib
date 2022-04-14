// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/**
 * Lazy log string values. This will only write if the value has been changed since last append call
 */
public class LazyStringLogEntry extends StringLogEntry {
  private String m_lastValue;
  private boolean m_firstAppend = true;

  public LazyStringLogEntry(
      DataLog log, String name, String metadata, String type, long timestamp) {
    super(log, name, metadata, type, timestamp);
  }

  public LazyStringLogEntry(DataLog log, String name, String metadata, String type) {
    super(log, name, metadata, type);
  }

  public LazyStringLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, metadata, timestamp);
  }

  public LazyStringLogEntry(DataLog log, String name, String metadata) {
    super(log, name, metadata);
  }

  public LazyStringLogEntry(DataLog log, String name, long timestamp) {
    super(log, name, timestamp);
  }

  public LazyStringLogEntry(DataLog log, String name) {
    super(log, name);
  }

  @Override
  public void append(String value, long timestamp) {
    if (m_firstAppend || !value.equals(m_lastValue)) {
      m_firstAppend = false;
      m_lastValue = value;
      super.append(value, timestamp);
    }
  }
}
