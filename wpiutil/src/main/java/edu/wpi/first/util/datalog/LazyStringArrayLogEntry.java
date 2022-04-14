// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Arrays;

/**
 * Lazy log array of string values. This will only write if the value has been changed since last
 * append call
 */
public class LazyStringArrayLogEntry extends StringArrayLogEntry {
  private String[] m_lastValue;
  private boolean m_firstAppend = true;

  public LazyStringArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, metadata, timestamp);
  }

  public LazyStringArrayLogEntry(DataLog log, String name, String metadata) {
    super(log, name, metadata);
  }

  public LazyStringArrayLogEntry(DataLog log, String name, long timestamp) {
    super(log, name, timestamp);
  }

  public LazyStringArrayLogEntry(DataLog log, String name) {
    super(log, name);
  }

  @Override
  public void append(String[] value, long timestamp) {
    if (m_firstAppend || !Arrays.equals(value, m_lastValue)) {
      m_firstAppend = false;
      m_lastValue = Arrays.copyOf(value, value.length);
      super.append(value, timestamp);
    }
  }
}
